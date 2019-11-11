#include "sprite.h"
#include <plog/Log.h>

static inline double grad(int dst, int src, double t)
{
    if (src == dst) return src;
    return dst * t + src * (1.0 - t);
}

////////////////////////////////////////////////////////////////////////////////
// virtual base class functions
vSprite::vSprite(pTexture tex, SpriteTypes type) :
    _pTexture(tex), _type(type), _current({ 0, RenderParams::CONSTANT, 0x00000000, BlendMode::NONE, false, 0 }) {}

bool vSprite::updateByKeyframes(timestamp rawTime)
{
    // Check if object is valid
	// Note that nullptr texture shall pass
    if (_pTexture != nullptr && !_pTexture->_loaded)
        return false;

    // Check if frames is valid
    size_t frameCount = _keyFrames.size();
    if (frameCount < 1)
        return false;

	timestamp time;


	// Check if timer is 140
	if (_triggerTimer == eTimer::MUSIC_BEAT)
		time = gTimers.get(eTimer::MUSIC_BEAT);
    else
    {
        // Check if timer is valid
        if (gTimers.get(_triggerTimer) < 0)
            return false;

        time = rawTime - gTimers.get(_triggerTimer);
    }
    
    // Check if import time is valid
	timestamp endTime = timestamp(_keyFrames[frameCount - 1].time);
    if (time.norm() < 0 || _loopTo < 0 && time > endTime)
        return false;

    // Check if loop target is valid
    if (_loopTo < 0 && time > endTime)
        return false;
    if (_loopTo > _keyFrames[frameCount - 1].time)
        time = _keyFrames[frameCount - 1].time;


    // crop time into valid section
    if (time > endTime)
    {
		if (endTime != _loopTo)
			time = timestamp((time - _loopTo).norm() % (endTime - _loopTo).norm() + _loopTo);
        else
            time = _loopTo;
    }

    // Check if specific time
    if (frameCount == 1 || time == _keyFrames[0].time)      
    {
        // exactly first frame
        _current = _keyFrames[0].param;
        return true;
    }
    else if (time == _keyFrames[frameCount - 1].time)       
    {
        // exactly last frame
        _current = _keyFrames[frameCount - 1].param;
        return true;
    }

    // get keyFrame section (iterators)
    decltype(_keyFrames.begin()) keyFrameCurr, keyFrameNext;
    for (auto it = _keyFrames.begin(); it != _keyFrames.end(); ++it)
    {
        if (it->time <= time.norm()) keyFrameCurr = it;
        else break;
    }
    keyFrameNext = keyFrameCurr;
    if (keyFrameCurr + 1 != _keyFrames.end()) ++keyFrameNext;

    // Check if section period is 0
    auto keyFrameLength = keyFrameNext->time - keyFrameCurr->time;
    if (keyFrameLength == 0)
    {
        _current = keyFrameCurr->param;
        return true;
    }

    // normalize time
    double t = 1.0 * (time.norm() - keyFrameCurr->time) / keyFrameLength;
    switch (keyFrameCurr->param.accel)
    {
    case RenderParams::CONSTANT:
        break;
    case RenderParams::ACCEL:
        t = std::pow(t, 2.0);
        break;
    case RenderParams::DECEL:
        t = std::pow(t, 0.5);
        break;
    case RenderParams::DISCONTINOUS:
        t = 0.0;
    }

    // calculate parameters
	_current.rect.x = (int)grad(keyFrameNext->param.rect.x, keyFrameCurr->param.rect.x, t);
	_current.rect.y = (int)grad(keyFrameNext->param.rect.y, keyFrameCurr->param.rect.y, t);
	_current.rect.w = (int)grad(keyFrameNext->param.rect.w, keyFrameCurr->param.rect.w, t);
	_current.rect.h = (int)grad(keyFrameNext->param.rect.h, keyFrameCurr->param.rect.h, t);
    //_current.rect  = keyFrameNext->param.rect  * t + keyFrameCurr->param.rect  * (1.0 - t);
	_current.color.r = (Uint8)grad(keyFrameNext->param.color.r, keyFrameCurr->param.color.r, t);
	_current.color.g = (Uint8)grad(keyFrameNext->param.color.g, keyFrameCurr->param.color.g, t);
	_current.color.b = (Uint8)grad(keyFrameNext->param.color.b, keyFrameCurr->param.color.b, t);
	_current.color.a = (Uint8)grad(keyFrameNext->param.color.a, keyFrameCurr->param.color.a, t);
    //_current.color = keyFrameNext->param.color * t + keyFrameNext->param.color * (1.0 - t);
	_current.angle = grad(keyFrameNext->param.angle, keyFrameNext->param.angle, t);
    //LOG_DEBUG << "[Skin] Time: " << time << 
    //    " @ " << _current.rect.x << "," << _current.rect.y << " " << _current.rect.w << "x" << _current.rect.h;
    //LOG_DEBUG<<"[Skin] keyFrameCurr: " << keyFrameCurr->param.rect.x << "," << keyFrameCurr->param.rect.y << " " << keyFrameCurr->param.rect.w << "x" << keyFrameCurr->param.rect.h;
    //LOG_DEBUG<<"[Skin] keyFrameNext: " << keyFrameNext->param.rect.x << "," << keyFrameNext->param.rect.y << " " << keyFrameNext->param.rect.w << "x" << keyFrameNext->param.rect.h;
	_current.blend = keyFrameCurr->param.blend;
	_current.filter = keyFrameCurr->param.filter;

    if (!_parent.expired())
    {
        auto parent = _parent.lock();
        _current.rect.x += parent->getCurrentRenderParams().rect.x;
        _current.rect.y += parent->getCurrentRenderParams().rect.y;
        _current.color.r = (Uint8)std::min(0.0, parent->getCurrentRenderParams().color.r / 255.0 * _current.color.r);
        _current.color.g = (Uint8)std::min(0.0, parent->getCurrentRenderParams().color.g / 255.0 * _current.color.g);
        _current.color.b = (Uint8)std::min(0.0, parent->getCurrentRenderParams().color.b / 255.0 * _current.color.b);
        _current.color.a = (Uint8)std::min(0.0, parent->getCurrentRenderParams().color.a / 255.0 * _current.color.a);
        _current.angle += parent->getCurrentRenderParams().angle;
    }
    
    return true;
}

bool vSprite::update(timestamp t)
{
    if (_haveParent && !_parent.lock()->_draw)
        return _draw = false;

	return _draw = updateByKeyframes(t);
}

RenderParams vSprite::getCurrentRenderParams()
{
    return _current;
}

void vSprite::setLoopTime(int t)
{
    _loopTo = t;
}

void vSprite::setTrigTimer(eTimer t)
{
	_triggerTimer = t;
}

void vSprite::appendKeyFrame(RenderKeyFrame f)
{
    _keyFrames.push_back(f);
}
void vSprite::appendInvisibleLeadingFrame()
{
    appendKeyFrame({ 0, {Rect(), RenderParams::accTy::DISCONTINOUS, Color(0), BlendMode::NONE, false, 0.0} });
}

////////////////////////////////////////////////////////////////////////////////
// Static

SpriteStatic::SpriteStatic(pTexture texture) :
	SpriteStatic(texture, texture ? texture->getRect(): Rect()) {}
SpriteStatic::SpriteStatic(pTexture texture, const Rect& rect):
    vSprite(texture, SpriteTypes::STATIC), _texRect(rect) {}

bool SpriteStatic::update(timestamp t)
{
	return vSprite::update(t);
}

void SpriteStatic::draw() const
{
    if (_draw && _pTexture->_loaded)
        _pTexture->draw(_texRect, _current.rect, _current.color, _current.blend, _current.filter, _current.angle);
}

////////////////////////////////////////////////////////////////////////////////
// Split

SpriteSelection::SpriteSelection(pTexture texture, unsigned rows, unsigned cols, bool v): 
    SpriteSelection(texture, texture ? texture->getRect() : Rect(), rows, cols, v)
{
}

SpriteSelection::SpriteSelection(pTexture texture, const Rect& r, unsigned rows, unsigned cols, bool v):
    vSprite(texture, SpriteTypes::SPLIT)
{
    if (rows == 0 || cols == 0)
    {
        _srows = _scols = 0;
        _texRect.resize(0);
        return;
    }

    _srows = rows;
    _scols = cols;
    _segments = rows * cols;
    auto rect = r;
    rect.w /= cols;
    rect.h /= rows;
    if (!v)
    {
        // Horizontal first
        for (unsigned r = 0; r < rows; ++r)
            for (unsigned c = 0; c < cols; ++c)
            {
                _texRect.emplace_back(
                    rect.x + rect.w * c,
                    rect.y + rect.h * r,
                    rect.w,
                    rect.h
                );
            }
    }
    else
    {
        // Vertical first
        for (unsigned c = 0; c < cols; ++c)
            for (unsigned r = 0; r < rows; ++r)
            {
                _texRect.emplace_back(
                    rect.x + rect.w * c,
                    rect.y + rect.h * r,
                    rect.w,
                    rect.h
                );
            }
    }
}

void SpriteSelection::draw() const
{
    if (_draw && _pTexture->_loaded)
        _pTexture->draw(_texRect[_selectionIdx], _current.rect, _current.color, _current.blend, _current.filter, _current.angle);
}

void SpriteSelection::updateSelection(frameIdx frame)
{
    _selectionIdx = frame < _segments ? frame : _segments - 1;
}

bool SpriteSelection::update(timestamp t)
{
	return vSprite::update(t);
}

////////////////////////////////////////////////////////////////////////////////
// Animated

SpriteAnimated::SpriteAnimated(pTexture texture, 
    unsigned animFrames, unsigned frameTime, eTimer t, 
    unsigned selRows, unsigned selCols, bool selVert):
    SpriteAnimated(texture, texture ? texture->getRect() : Rect(), animFrames, frameTime, t,
		selRows, selCols, selVert)
{
}

SpriteAnimated::SpriteAnimated(pTexture texture, const Rect& r, 
    unsigned animFrames, unsigned frameTime, eTimer t, 
    unsigned selRows, unsigned selCols, bool selVert):
    SpriteSelection(texture, r, selRows, selCols, selVert), _animFrames(animFrames), _resetAnimTimer(t)
{
    _type = SpriteTypes::ANIMATED;

    if (animFrames == 0 || selRows == 0 || selCols == 0) return;

	if (_animFrames != 0) _selections = selRows * selCols / _animFrames;
	//_aframes = animFrames;
    //_aRect.w = _texRect[0].w / animCols;
    //_aRect.h = _texRect[0].h / animRows;
    //_arows = animRows;
    //_acols = animCols;
    //_aframes = animRows * animCols;
    _period = frameTime;
    //_aVert = animVert;
}

bool SpriteAnimated::update(timestamp t)
{
	if (SpriteSelection::update(t))
	{
		updateByTimer(t);
		//updateSplitByTimer(t);
		updateAnimationByTimer(t);
		return true;
	}
	return false;
}

void SpriteAnimated::updateByTimer(timestamp time)
{
	if (gTimers.get(_triggerTimer))
		updateByKeyframes(time - timestamp(gTimers.get(_triggerTimer)));
}

void SpriteAnimated::updateAnimation(timestamp time)
{
    if (_segments == 0) return;
    if (_period == -1) return;

    if (double timeEachFrame = double(_period) / _animFrames; timeEachFrame >= 1.0)
    {
        auto animFrameTime = time.norm() % _period;
        _currAnimFrame = std::floor(animFrameTime / timeEachFrame);
    }
	/*
    _drawRect = _texRect[_selectionIdx];
    _drawRect.w = _aRect.w;
    _drawRect.h = _aRect.h;
    if (!_aVert)
    {
        // Horizontal first
        _drawRect.x += _aRect.w * (f % _acols);
        _drawRect.y += _aRect.h * (f / _acols);
    }
    else
    {
        // Vertical first
        _drawRect.x += _aRect.w * (f / _arows);
        _drawRect.y += _aRect.h * (f % _arows);
    }
	*/
}

void SpriteAnimated::updateAnimationByTimer(timestamp time)
{
	if (gTimers.get(_resetAnimTimer))
		updateAnimation(time - timestamp(gTimers.get(_resetAnimTimer)));
}

// Commented for backup purpose. I don't think I can understand this...
// Animation should not affect Split rect, which is decided by user.
/*
void SpriteAnimated::updateSplitByTimer(rTime time)
{
    // total frame:    _aframes
    // time one cycle: _period
    // time per frame: _period / _aframes
    // current time:   t
    // current frame:  t / (_period / _aframes)
    if (_period / _aframes > 0 && gTimers.get(_triggerTimer))
        updateSplit((frameIdx)((time - gTimers.get(_triggerTimer)) / (_period / _aframes)));
}
*/
void SpriteAnimated::draw() const
{
    if (_draw && _pTexture != nullptr && _pTexture->_loaded)
    {
        _pTexture->draw(_texRect[_selectionIdx * _animFrames + _currAnimFrame], _current.rect, _current.color, _current.blend, _current.filter, _current.angle);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Text

SpriteText::SpriteText(pFont f, eText e, TextAlign a, unsigned ptsize, Color c):
   SpriteStatic(nullptr), _pFont(f), _textInd(e), _align(a), _color(c)
{
    _type = SpriteTypes::TEXT;
}

/*
SpriteText::SpriteText(pFont f, Rect rect, eText e, TextAlign a, unsigned ptsize, Color c):
   SpriteStatic(nullptr), _pFont(f), _frameRect(rect), _textInd(e), _align(a), _color(c)
{
    _opType = SpriteTypes::TEXT;
    _haveRect = true;
	_texRect = rect;
}
*/

bool SpriteText::update(timestamp t)
{
	if (updateByKeyframes(t))
	{
		setText(gTexts.get(_textInd), _current.color);
		updateTextRect();
		return true;
	}
	return false;
}

void SpriteText::updateTextRect()
{
	// fitting
	Rect textRect = _texRect;
	double sizeFactor = (double)_current.rect.h / textRect.h;
	int text_w = textRect.w * sizeFactor;
	double widthFactor = (double)_current.rect.w / text_w;
	if (widthFactor > 1.0)
	{
		switch (_align)
		{
		case TEXT_ALIGN_LEFT:
			break;
		case TEXT_ALIGN_CENTER:
			_current.rect.x += (_current.rect.w - text_w) / 2;
			break;
		case TEXT_ALIGN_RIGHT:
			_current.rect.x += (_current.rect.w - text_w);
			break;
		}
		_current.rect.w = text_w;
	}
}

void SpriteText::setText(std::string text, const Color& c)
{
    if (!_pFont->_loaded) return;
    if (!_pTexture) return;
    if (_currText == text && _color == c) return;
    _currText = text;
    _color = c;
    _pTexture = _pFont->TextUTF8(_currText.c_str(), c);
	_texRect = _pTexture->getRect();
}

void SpriteText::draw() const
{
	if (_pTexture)
		SpriteStatic::draw();
}


SpriteNumber::SpriteNumber(pTexture texture, NumberAlign align, unsigned maxDigits,
    unsigned numRows, unsigned numCols, unsigned frameTime, eNumber n, eTimer t,
    unsigned animFrames, bool numVert):
    SpriteNumber(texture, texture ? texture->getRect() : Rect(), align, maxDigits,
		numRows, numCols, frameTime, n, t,
		animFrames, numVert)
{
}

SpriteNumber::SpriteNumber(pTexture texture, const Rect& rect, NumberAlign align, unsigned maxDigits,
    unsigned numRows, unsigned numCols, unsigned frameTime, eNumber n, eTimer t,
    unsigned animFrames, bool numVert):
    SpriteAnimated(texture, rect, animFrames, frameTime, t, numRows, numCols, numVert),
    _alignType(align), _numInd(n), _maxDigits(maxDigits)
{
    _type = SpriteTypes::NUMBER;

    // invalid num type guard
    //_numType = NumberType(numRows * numCols);
	if (animFrames != 0) _numType = NumberType(numRows * numCols / animFrames);
    switch (_numType)
    {
    case NUM_TYPE_NORMAL:
    case NUM_TYPE_BLANKZERO:
        break;
    //case NUM_SYMBOL:
    case NUM_TYPE_FULL: 
        break;
    default: return;
    }

    _digit.resize(maxDigits);
    _rects.resize(maxDigits);
}

bool SpriteNumber::update(timestamp t)
{
    if (_maxDigits == 0) return false;
	if (SpriteAnimated::update(t))
	{
        updateNumberByInd();

        switch (_alignType)
        {
        case NUM_ALIGN_RIGHT:
        {
            Rect offset{ int(_current.rect.w * (_maxDigits - 1)),0,0,0 };
            for (size_t i = 0; i < _maxDigits; ++i)
            {
                _rects[i] = _current.rect + offset;
                offset.x -= _current.rect.w;
            }
            break;
        }

        case NUM_ALIGN_LEFT:
        {
            Rect offset{ int(_current.rect.w * (_numDigits - 1)),0,0,0 };
            for (size_t i = 0; i < _numDigits; ++i)
            {
                _rects[i] = _current.rect + offset;
                offset.x -= _current.rect.w;
            }
            break;
        }

        case NUM_ALIGN_CENTER:
        {
            double delta = 0.5 * _current.rect.w;
            Rect offset{ 0,0,0,0 };
            if (_inhibitZero)
                offset.x = int(std::floor(delta * (_numDigits - 1)));
            else
                offset.x = int(std::floor(delta * (_maxDigits - _numDigits)));
            for (size_t i = 0; i < _numDigits; ++i)
            {
                _rects[i] = _current.rect + offset;
                offset.x -= _current.rect.w;
            }
            break;
        }
        }
		return true;
	}
	return false;
}

void SpriteNumber::updateNumber(int n)
{
    bool positive = n >= 0;
	size_t zeroIdx = 0;
    unsigned maxDigits = _digit.size();
	switch (_numType)
	{
	case NUM_TYPE_NORMAL:    zeroIdx = 0; break;
	case NUM_TYPE_BLANKZERO: zeroIdx = NUM_BZERO; break;
    case NUM_TYPE_FULL:      zeroIdx = positive ? NUM_FULL_BZERO_POS : NUM_FULL_BZERO_NEG; maxDigits--; break;
	}

    // reset by zeroIdx to prevent unexpected glitches
    for (auto& d : _digit) d = zeroIdx;

	if (n == 0)
	{
        _digit[0] = 0;
		_numDigits = 1;
	}
	else
	{
		_numDigits = 0;
		int abs_n = std::abs(n);
		for (unsigned i = 0; abs_n && i < maxDigits; ++i)
		{
			++_numDigits;
			unsigned digit = abs_n % 10;
			abs_n /= 10;
            if (_numType == NUM_TYPE_FULL && !positive) digit += 12;
			_digit[i] = digit;
		}
	}

    // symbol
    switch (_numType)
    {
		/*
        case NUM_SYMBOL:
        {
            _digit[_sDigit.size() - 1] = positive ? NUM_SYMBOL_PLUS : NUM_SYMBOL_MINUS;
            break;
        }
		*/
        case NUM_TYPE_FULL:
        {
            switch (_alignType)
            {

            case NUM_ALIGN_RIGHT:
                if (!_inhibitZero || _numDigits == _maxDigits)
                    _numDigits = _maxDigits - 1;
                _digit[_numDigits++] = positive ? NUM_FULL_PLUS : NUM_FULL_MINUS;
                break;

            case NUM_ALIGN_LEFT:
            case NUM_ALIGN_CENTER: 
                if (_numDigits == _maxDigits)
                    --_numDigits;
                _digit[_numDigits++] = positive ? NUM_FULL_PLUS : NUM_FULL_MINUS;
                break;
            }
            break;
        }
    }
}

void SpriteNumber::updateNumberByInd()
{
    int n;
    switch (_numInd)
    {
    case eNumber::RANDOM:
        n = std::rand();
        break;
    case eNumber::ZERO:
        n = 0;
        break;
	case (eNumber)10220:
		n = timestamp().norm();
		break;
    default:
#ifdef _DEBUG
		n = (int)_numInd >= 10000 ? (int)gTimers.get((eTimer)((int)_numInd - 10000)) : gNumbers.get(_numInd);
#else
        n = gNumbers.get(_numInd);
#endif
        break;
    }
    updateNumber(n);
}

void SpriteNumber::appendKeyFrame(RenderKeyFrame f)
{
    _keyFrames.push_back(f);
}

void SpriteNumber::draw() const
{
    if (_pTexture->_loaded && _draw)
    {
        //for (size_t i = 0; i < _outRectDigit.size(); ++i)
        //    _pTexture->draw(_drawRectDigit[i], _outRectDigit[i], _current.angle);

        size_t max = 0;
        switch (_alignType)
        {
        case NUM_ALIGN_RIGHT:
            max = _inhibitZero ? _numDigits : _maxDigits;
            break;
        case NUM_ALIGN_LEFT:
        case NUM_ALIGN_CENTER:
            max = _numDigits;
            break;
        default:
            break;
        }
        for (size_t i = 0; i < max; ++i)
        {
            _pTexture->draw(_texRect[_currAnimFrame * _selections + _digit[i]], _rects[i],
                _current.color, _current.blend, _current.filter, _current.angle);
        }
    }
}

SpriteSlider::SpriteSlider(pTexture texture, SliderDirection d, int range,
	unsigned animFrames, unsigned frameTime, eSlider ind, eTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteSlider(texture, texture ? texture->getRect() : Rect(), d, range,
		animFrames, frameTime, ind, timer,
		selRows, selCols, selVerticalIndexing) {}

SpriteSlider::SpriteSlider(pTexture texture, const Rect& rect, SliderDirection d, int range,
	unsigned animFrames, unsigned frameTime, eSlider ind, eTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteAnimated(texture, rect, animFrames, frameTime, timer,
		selRows, selCols, selVerticalIndexing), _ind(ind), _dir(d), _range(range)
{
	_type = SpriteTypes::SLIDER;
}

void SpriteSlider::updateVal(percent v)
{
	_value = v;
}

void SpriteSlider::updateValByInd()
{
	updateVal(gSliders.get(_ind));
}

void SpriteSlider::updatePos()
{
	int pos_delta = (_range-1) * _value;
	switch (_dir)
	{
	case SliderDirection::DOWN:
		_current.rect.y += pos_delta;
		break;
	case SliderDirection::UP:
		_current.rect.y -= pos_delta;
		break;
	case SliderDirection::RIGHT:
		_current.rect.x += pos_delta;
		break;
	case SliderDirection::LEFT:
		_current.rect.x -= pos_delta;
		break;
	}
}

bool SpriteSlider::update(timestamp t)
{
	if (SpriteAnimated::update(t))
	{
		updateValByInd();
		updatePos();
		return true;
	}
	return false;
}

SpriteBargraph::SpriteBargraph(pTexture texture, BargraphDirection d,
	unsigned animFrames, unsigned frameTime, eBargraph ind, eTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteBargraph(texture, texture ? texture->getRect() : Rect(), d,
		animFrames, frameTime, ind, timer,
		selRows, selCols, selVerticalIndexing) {}

SpriteBargraph::SpriteBargraph(pTexture texture, const Rect& rect, BargraphDirection d,
	unsigned animFrames, unsigned frameTime, eBargraph ind, eTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteAnimated(texture, rect, animFrames, frameTime, timer,
		selRows, selCols, selVerticalIndexing), _dir(d), _ind(ind)
{
	_type = SpriteTypes::BARGRAPH;
}

void SpriteBargraph::updateVal(dpercent v)
{
	_value = v;
}

void SpriteBargraph::updateValByInd()
{
	updateVal(gBargraphs.get(_ind));
}

#pragma warning(push)
#pragma warning(disable: 4244)
void SpriteBargraph::updateSize()
{
	int tmp;
	switch (_dir)
	{
	case BargraphDirection::DOWN:
		_current.rect.h *= _value;
		break;
	case BargraphDirection::UP:
		tmp = _current.rect.h;
		_current.rect.h *= _value;
		_current.rect.y += tmp - _current.rect.h;
		break;
	case BargraphDirection::RIGHT:
		_current.rect.w *= _value;
		break;
	case BargraphDirection::LEFT:
		tmp = _current.rect.w;
		_current.rect.w *= _value;
		_current.rect.x += tmp - _current.rect.w;
		break;
	}
}
#pragma warning(pop)

bool SpriteBargraph::update(timestamp t)
{
	if (SpriteAnimated::update(t))
	{
		updateValByInd();
		updateSize();
		return true;
	}
	return false;
}

SpriteOption::SpriteOption(pTexture texture,
	unsigned animFrames, unsigned frameTime, eTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteOption(texture, texture ? texture->getRect() : Rect(),
		animFrames, frameTime, timer,
		selRows, selCols, selVerticalIndexing) {}

SpriteOption::SpriteOption(pTexture texture, const Rect& rect,
	unsigned animFrames, unsigned frameTime, eTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteAnimated(texture, rect, animFrames, frameTime, timer,
		selRows, selCols, selVerticalIndexing)
{
	_type = SpriteTypes::OPTION;
}

bool SpriteOption::setInd(opType type, unsigned ind)
{
	if (_opType != opType::UNDEF) return false;
	switch (type)
	{
	case opType::UNDEF:
		return false;

	case opType::OPTION:
		_opType = opType::OPTION;
		_ind.op = (eOption)ind;
		return true;

	case opType::SWITCH:
		_opType = opType::SWITCH;
		_ind.sw = (eSwitch)ind;
		return true;
	}
	return false;
}

void SpriteOption::updateVal(unsigned v)
{
	_value = v;
	updateSelection(v);
}

void SpriteOption::updateValByInd()
{
	switch (_opType)
	{
	case opType::UNDEF:
		break;

	case opType::OPTION:
		updateVal(gOptions.get(_ind.op));
		break;

	case opType::SWITCH:
		updateVal(gSwitches.get(_ind.sw));
		break;
	}
}

bool SpriteOption::update(timestamp t)
{
	if (SpriteSelection::update(t))
	{
		updateValByInd();
		return true;
	}
	return false;
}


SpriteGaugeGrid::SpriteGaugeGrid(pTexture texture,
	unsigned animFrames, unsigned frameTime, int dx, int dy, unsigned min, unsigned max, unsigned grids,
	eTimer timer, eNumber num, unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteGaugeGrid(texture, texture ? texture->getRect() : Rect(), animFrames, frameTime, 
        dx, dy, grids, min, max, timer, num, selRows, selCols, selVerticalIndexing) {}

SpriteGaugeGrid::SpriteGaugeGrid(pTexture texture, const Rect& rect,
	unsigned animFrames, unsigned frameTime,  int dx, int dy, unsigned min, unsigned max, unsigned grids,
	eTimer timer, eNumber num, unsigned selRows, unsigned selCols, bool selVerticalIndexing): 
	SpriteAnimated(texture, rect, animFrames, frameTime, timer, selRows, selCols, selVerticalIndexing),
	_delta_x(dx), _delta_y(dy), _grids(grids), _min(min), _max(max), _numInd(num)
{
    _lighting.resize(_grids, false);
    setGaugeType(GaugeType::GROOVE);
}

void SpriteGaugeGrid::setFlashType(SpriteGaugeGrid::FlashType t)
{
	_flashType = t;
}

void SpriteGaugeGrid::setGaugeType(SpriteGaugeGrid::GaugeType ty)
{
	_gaugeType = ty;
	switch (_gaugeType)
	{
    case GaugeType::GROOVE: 
        _texIdxLightFail = NORMAL_LIGHT; _texIdxDarkFail = NORMAL_DARK; 
        _texIdxLightClear = CLEAR_LIGHT; _texIdxDarkClear = CLEAR_DARK;
        _req = (unsigned short)std::floor(0.8 * _grids); break;

    case GaugeType::SURVIVAL:  
        _texIdxLightFail = CLEAR_LIGHT; _texIdxDarkFail = CLEAR_DARK;
        _texIdxLightClear = CLEAR_LIGHT; _texIdxDarkClear = CLEAR_DARK;
        _req = 1; break;

    case GaugeType::EX_SURVIVAL: 
        _texIdxLightFail = EXHARD_LIGHT; _texIdxDarkFail = EXHARD_DARK;
        _texIdxLightClear = EXHARD_LIGHT; _texIdxDarkClear = EXHARD_DARK;
        _req = 1; break;
	default: break;
	}

    timestamp t(1);

    // set FailRect
    updateSelection(_texIdxLightFail);
    SpriteAnimated::update(t);
    _lightRectFailIdxOffset = _selectionIdx * _animFrames;
    updateSelection(_texIdxDarkFail);
    SpriteAnimated::update(t);
    _darkRectFailIdxOffset = _selectionIdx * _animFrames;

    // set ClearRect
    updateSelection(_texIdxLightClear);
    SpriteAnimated::update(t);
    _lightRectClearIdxOffset = _selectionIdx * _animFrames;
    updateSelection(_texIdxDarkClear);
    SpriteAnimated::update(t);
    _darkRectClearIdxOffset = _selectionIdx * _animFrames;
}

void SpriteGaugeGrid::updateVal(unsigned v)
{
	_val = _grids * (v - _min) / (_max - _min);
}

void SpriteGaugeGrid::updateValByInd()
{
	updateVal(gNumbers.get(_numInd));
}

bool SpriteGaugeGrid::update(timestamp t)
{
	if (SpriteAnimated::update(t))
	{
        updateValByInd();
		switch (_flashType)
		{
		case FlashType::NONE:
			for (unsigned i = 0; i < _val; ++i)
				_lighting[i] = true;
			for (unsigned i = _val; i < _grids; ++i)
				_lighting[i] = false;
			break;

		case FlashType::CLASSIC:
			for (unsigned i = 0; i < _val; ++i)
				_lighting[i] = true;
			if (_val - 3 >= 0 && _val - 3 < _grids && t.norm() / 17 % 2) _lighting[_val - 3] = false; // -3 grid: 17ms, per 2 units (1 0 1 0)
			if (_val - 2 >= 0 && _val - 2 < _grids && t.norm() / 17 % 4) _lighting[_val - 2] = false; // -2 grid: 17ms, per 4 units (1 0 0 0)
			for (unsigned i = _val; i < _grids; ++i)
				_lighting[i] = false;
			break;
			
		default: break;
		}
		return true;
	}
	return false;
}

void SpriteGaugeGrid::draw() const
{
    if (_draw && _pTexture != nullptr && _pTexture->isLoaded())
    {
		Rect r = _current.rect;
        for (unsigned i = 0; i < _req - 1; ++i)
        {
            _lighting[i] ?
                _pTexture->draw(_texRect[_lightRectFailIdxOffset + _currAnimFrame], r, _current.color, _current.blend, _current.filter, _current.angle) :
                _pTexture->draw(_texRect[_darkRectFailIdxOffset + _currAnimFrame], r, _current.color, _current.blend, _current.filter, _current.angle);
            r.x += _delta_x;
            r.y += _delta_y;
        }
        for (unsigned i = _req - 1; i < _grids; ++i)
        {
            _lighting[i] ?
                _pTexture->draw(_texRect[_lightRectClearIdxOffset + _currAnimFrame], r, _current.color, _current.blend, _current.filter, _current.angle) :
                _pTexture->draw(_texRect[_darkRectClearIdxOffset + _currAnimFrame], r, _current.color, _current.blend, _current.filter, _current.angle);
            r.x += _delta_x;
            r.y += _delta_y;
        }
    }
}

SpriteLine::SpriteLine(int width, Color color) : SpriteStatic(nullptr), _line(width), _color(color)
{
	_type = SpriteTypes::LINE;
}

void SpriteLine::appendPoint(const ColorPoint& c) { _points.push_back(c); }

void SpriteLine::draw() const
{
	size_t m = (size_t)std::floor((_points.size() - 1) * _progress);
	for (size_t i = 0; i < m; ++i)
	{
		_line.draw(_points[i].p, _points[i+1].p, _points[i].c);
	}
}

void SpriteLine::updateProgress(timestamp t)
{
	_progress = (double)(gTimers.get(_triggerTimer) - t.norm() - _timerStartOffset) / _duration;
	_progress = std::clamp(_progress, 0.0, 1.0);
}

bool SpriteLine::update(timestamp t)
{
	if (SpriteStatic::update(t))
	{
		updateProgress(t);
		return true;
	}
	return false;
}

