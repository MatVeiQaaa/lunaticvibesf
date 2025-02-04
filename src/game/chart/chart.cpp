#include "chart.h"
#include "chart_types.h"
#include "common/chartformat/chartformat.h"
#include "common/chartformat/chartformat_types.h"
#include "common/log.h"
#include "game/runtime/state.h"
#include "game/scene/scene_context.h"
#include <common/assert.h>
#include <iterator>

using namespace chart;

// from NoteLaneIndex to Input::Pad
const std::vector<Input::Pad>& chart::LaneToKey(int keys, size_t idx)
{
    LVF_DEBUG_ASSERT(idx < 26);
    using namespace Input;
    switch (keys)
    {
    case 5:
    case 10: {
        static const std::vector<Input::Pad> pad[] = {
            {Input::Pad::S1L, Input::Pad::S1R}, // Sc1
            {Input::Pad::K11},
            {Input::Pad::K12},
            {Input::Pad::K13},
            {Input::Pad::K14},
            {Input::Pad::K15},
            {},
            {},
            {},
            {},
            {Input::Pad::K21},
            {Input::Pad::K22},
            {Input::Pad::K23},
            {Input::Pad::K24},
            {Input::Pad::K25},
            {},
            {},
            {},
            {},
            {Input::Pad::S2L, Input::Pad::S2R} // Sc2
        };
        return pad[idx];
    }
    case 7:
    case 14: {
        static const std::vector<Input::Pad> pad[] = {
            {Input::Pad::S1L, Input::Pad::S1R}, // Sc1
            {Input::Pad::K11},
            {Input::Pad::K12},
            {Input::Pad::K13},
            {Input::Pad::K14},
            {Input::Pad::K15},
            {Input::Pad::K16},
            {Input::Pad::K17},
            {},
            {},
            {Input::Pad::K21},
            {Input::Pad::K22},
            {Input::Pad::K23},
            {Input::Pad::K24},
            {Input::Pad::K25},
            {Input::Pad::K26},
            {Input::Pad::K27},
            {},
            {},
            {Input::Pad::S2L, Input::Pad::S2R} // Sc2
        };
        return pad[idx];
    }
    case 9: {
        static const std::vector<Input::Pad> pad[] = {
            {}, // Sc1
            {Input::Pad::K11},
            {Input::Pad::K12},
            {Input::Pad::K13},
            {Input::Pad::K14},
            {Input::Pad::K15},
            {Input::Pad::K16},
            {Input::Pad::K17},
            {Input::Pad::K18},
            {Input::Pad::K19},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {} // Sc2
        };
        return pad[idx];
    }
    }
    static const std::vector<Input::Pad> empty{};
    return empty;
}

// from Input::Pad to NoteLaneIndex
chart::NoteLaneIndex chart::KeyToLane(int keys, Input::Pad pad)
{
    LVF_DEBUG_ASSERT(pad < Input::Pad::LANE_COUNT);

    using namespace chart;
    switch (keys)
    {
    case 5:
    case 10: {
        static constexpr NoteLaneIndex lane[4][Input::Pad::LANE_COUNT] = {
            {
                Sc1, Sc1, N11, N12, N13, N14, N15, _, _, _, _, _, _, _, _,
                Sc2, Sc2, N21, N22, N23, N24, N25, _, _, _, _, _, _, _, _,
            },
            {
                Sc1, Sc1, N11, N12, N13, N14, N15, _,   _,   _, _, _, _, _, _,
                Sc2, Sc2, _,   _,   N21, N22, N23, N24, N25, _, _, _, _, _, _,
            },
            {
                Sc1, Sc1, _,   _,   N11, N12, N13, N14, N15, _, _, _, _, _, _,
                Sc2, Sc2, N21, N22, N23, N24, N25, _,   _,   _, _, _, _, _, _,
            },
            {
                Sc1, Sc1, _, _, N11, N12, N13, N14, N15, _, _, _, _, _, _,
                Sc2, Sc2, _, _, N21, N22, N23, N24, N25, _, _, _, _, _, _,
            },
        };
        const auto fiveKeyMapIndex = gPlayContext.shiftFiveKeyForSevenKeyIndex(true);
        LVF_DEBUG_ASSERT(fiveKeyMapIndex >= 0);
        LVF_DEBUG_ASSERT(fiveKeyMapIndex < static_cast<int>(std::size(lane)));
        return lane[fiveKeyMapIndex][pad];
    }
    case 7:
    case 14: {
        static constexpr NoteLaneIndex lane[Input::Pad::LANE_COUNT] = {
            Sc1, Sc1, N11, N12, N13, N14, N15, N16, N17, _, _, _, _, _, _,
            Sc2, Sc2, N21, N22, N23, N24, N25, N26, N27, _, _, _, _, _, _,
        };
        return lane[pad];
    }
    case 9: {
        static constexpr NoteLaneIndex lane[Input::Pad::LANE_COUNT] = {
            _, _, N11, N12, N13, N14, N15, N16, N17, N18, N19, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
        };
        return lane[pad];
    }
    }
    return _;
}

ChartObjectBase::ChartObjectBase(int slot, size_t pn, size_t en)
    : _playerSlot(slot), _noteLists{}, _bgmNoteLists(pn), _specialNoteLists(en), _bgmNoteListIters(pn),
      _specialNoteListIters(en)
{
    reset();
    _bpmNoteList.clear();
    //_bpmList.push_back({ 0, {0, 1}, 0, 130 });
    //_stopList.push_back({ 0, {0, 1}, 0.0, 0, 1.0 });
}

std::shared_ptr<ChartObjectBase> ChartObjectBase::createFromChartFormat(int slot,
                                                                        const std::shared_ptr<ChartFormatBase>& p)
{
    switch (p->type())
    {
    case eChartFormat::BMS:
        try
        {
            return std::static_pointer_cast<ChartObjectBase>(
                std::make_shared<ChartObjectBMS>(slot, *std::reinterpret_pointer_cast<ChartFormatBMS>(p)));
        }
        catch (std::exception& e)
        {
            LOG_ERROR << "[chart] Load chart exception (" << e.what() << "): " << p->fileName;
            return nullptr;
        }
    default: LOG_WARNING << "[chart] Chart type unknown (" << int(p->type()) << "): " << p->fileName; return nullptr;
    }
}

void ChartObjectBase::reset()
{
    _currentBarTemp = 0;
    _currentMetreTemp = 0;
    _currentBPM = _bpmNoteList.empty() ? 150 : BPM(_bpmNoteList.front().fvalue);
    _currentBeatLength = lunaticvibes::Time::singleBeatLengthFromBPM(_currentBPM);
    _lastChangedBPMTime = 0;
    _lastChangedBPMMetre = 0;

    // reset notes
    for (auto& ch : _noteLists)
        for (auto& n : ch)
        {
            n.hit = false;
            n.expired = false;
        }

    // reset iterators
    resetNoteListsIterators();
}

void ChartObjectBase::resetNoteListsIterators()
{
    for (size_t i = 0; i < _noteLists.size(); ++i)
        _noteListIterators[i] = _noteLists[i].begin();
    for (size_t i = 0; i < _bgmNoteLists.size(); ++i)
        _bgmNoteListIters[i] = _bgmNoteLists[i].begin();
    for (size_t i = 0; i < _specialNoteLists.size(); ++i)
        _specialNoteListIters[i] = _specialNoteLists[i].begin();
    _bpmNoteListIter = _bpmNoteList.begin();
}

auto ChartObjectBase::firstNote(NoteLaneCategory cat, size_t idx) -> NoteIterator
{
    size_t channel = channelToIdx(cat, idx);
    return _noteLists[channel].begin();
}

auto ChartObjectBase::incomingNote(NoteLaneCategory cat, size_t idx) -> NoteIterator
{
    size_t channel = channelToIdx(cat, idx);
    return _noteListIterators[channel];
}

auto ChartObjectBase::incomingNoteBgm(size_t channel) -> decltype(_bgmNoteLists)::value_type::iterator
{
    return _bgmNoteListIters[channel];
}
auto ChartObjectBase::incomingNoteSpecial(size_t channel) -> decltype(_specialNoteLists)::value_type::iterator
{
    return _specialNoteListIters[channel];
}
auto ChartObjectBase::incomingNoteBpm() -> decltype(_bpmNoteList)::iterator
{
    return _bpmNoteListIter;
}
bool ChartObjectBase::isLastNote(NoteLaneCategory cat, size_t idx, const NoteIterator& it) const
{
    size_t channel = channelToIdx(cat, idx);
    return _noteLists[channel].empty() || it == _noteLists[channel].end();
}
bool ChartObjectBase::isLastNoteBgm(size_t idx, const decltype(_bgmNoteLists)::value_type::iterator& it) const
{
    return _bgmNoteLists[idx].empty() || it == _bgmNoteLists[idx].end();
}
bool ChartObjectBase::isLastNoteSpecial(size_t idx, const decltype(_specialNoteLists)::value_type::iterator& it) const
{
    return _specialNoteLists[idx].empty() || it == _specialNoteLists[idx].end();
}
bool ChartObjectBase::isLastNoteBpm(const decltype(_bpmNoteList)::iterator& it) const
{
    return _bpmNoteList.empty() || it == _bpmNoteList.end();
}

bool ChartObjectBase::isLastNote(NoteLaneCategory cat, size_t idx)
{
    return isLastNote(cat, idx, incomingNote(cat, idx));
}
bool ChartObjectBase::isLastNoteBgm(size_t channel)
{
    return isLastNoteBgm(channel, incomingNoteBgm(channel));
}
bool ChartObjectBase::isLastNoteSpecial(size_t channel)
{
    return isLastNoteSpecial(channel, incomingNoteSpecial(channel));
}
bool ChartObjectBase::isLastNoteBpm()
{
    return isLastNoteBpm(incomingNoteBpm());
}

auto ChartObjectBase::nextNote(NoteLaneCategory cat, size_t idx) -> NoteIterator&
{
    size_t channel = channelToIdx(cat, idx);
    return ++_noteListIterators[channel];
}

auto ChartObjectBase::nextNoteBgm(size_t channel) -> decltype(_bgmNoteLists)::value_type::iterator&
{
    return ++_bgmNoteListIters[channel];
}
auto ChartObjectBase::nextNoteSpecial(size_t channel) -> decltype(_specialNoteLists)::value_type::iterator&
{
    return ++_specialNoteListIters[channel];
}
auto ChartObjectBase::nextNoteBpm() -> decltype(_bpmNoteList)::iterator&
{
    return ++_bpmNoteListIter;
}

lunaticvibes::Time ChartObjectBase::getBarLength(size_t bar)
{
    if (bar + 1 >= _barTimestamp.size())
    {
        return -1;
    }

    auto l = _barTimestamp[bar + 1] - _barTimestamp[bar];
    return l.hres() > 0 ? l : -1;
}

lunaticvibes::Time ChartObjectBase::getCurrentBarLength()
{
    return getBarLength(_currentBarTemp);
}

Metre ChartObjectBase::getBarMetre(size_t bar)
{
    if (bar >= barMetreLength.size())
    {
        return {1, 1};
    }

    return barMetreLength[bar];
}

Metre ChartObjectBase::getCurrentBarMetre()
{
    return getBarMetre(_currentBarTemp);
}

Metre ChartObjectBase::getBarMetrePosition(size_t bar)
{
    if (bar >= _barMetrePos.size())
    {
        return Metre(LLONG_MAX, 1);
    }

    return _barMetrePos[bar];
}

void ChartObjectBase::update(const lunaticvibes::Time& rt)
{
    lunaticvibes::Time vt = rt + lunaticvibes::Time(State::get(IndexNumber::TIMING_ADJUST_VISUAL), false);
    const lunaticvibes::Time& at = rt;

    noteExpired.clear();
    noteBgmExpired.clear();
    noteSpecialExpired.clear();

    preUpdate(vt);

    // Go through expired measures
    while (_currentBarTemp + 1 < _barTimestamp.size() && vt >= _barTimestamp[_currentBarTemp + 1])
    {
        ++_currentBarTemp;
        _currentMetreTemp = 0;
        _lastChangedBPMTime = 0;
        _lastChangedBPMMetre = 0;
    }

    // check inbounds BPM change
    auto b = incomingNoteBpm();
    while (!isLastNoteBpm(b) && vt >= b->time)
    {
        //_currentMetreTemp = b->pos - getCurrentMeasureBeat();
        _currentBPM = BPM(b->fvalue);
        _currentBeatLength = lunaticvibes::Time::singleBeatLengthFromBPM(_currentBPM);
        _lastChangedBPMTime = b->time - _barTimestamp[_currentBarTemp];
        _lastChangedBPMMetre = b->pos - _barMetrePos[_currentBarTemp];
        b = nextNoteBpm();
    }

    // Skip expired notes
    for (auto cat_i = static_cast<size_t>(NoteLaneCategory::Note);
         cat_i < static_cast<size_t>(NoteLaneCategory::NOTECATEGORY_COUNT); ++cat_i)
    {
        const auto cat = static_cast<NoteLaneCategory>(cat_i);
        for (size_t idx_i = Sc1; idx_i < NOTELANEINDEX_COUNT; ++idx_i)
        {
            const NoteLaneIndex idx{idx_i};
            auto it = incomingNote(cat, idx);
            while (!isLastNote(cat, idx, it) && vt >= it->time && it->expired)
            {
                noteExpired.push_back(*it);
                it = nextNote(cat, idx);
            }
        }
    }

    // Skip expired barline
    {
        NoteLaneCategory cat = NoteLaneCategory::EXTRA;
        NoteLaneIndex idx = (NoteLaneIndex)EXTRA_BARLINE_1P;
        auto it = incomingNote(cat, idx);
        while (!isLastNote(cat, idx, it) && vt >= it->time)
        {
            it->expired = true;
            it = nextNote(cat, idx);
        }
        idx = (NoteLaneIndex)EXTRA_BARLINE_2P;
        it = incomingNote(cat, idx);
        while (!isLastNote(cat, idx, it) && vt >= it->time)
        {
            it->expired = true;
            it = nextNote(cat, idx);
        }
    }

    // Skip expired plain note
    for (size_t idx = 0; idx < _bgmNoteLists.size(); ++idx)
    {
        auto it = incomingNoteBgm(idx);
        while (!isLastNoteBgm(idx) && at >= it->time)
        {
            noteBgmExpired.push_back(*it);
            it = nextNoteBgm(idx);
        }
    }
    // Skip expired extended note
    for (size_t idx = 0; idx < _specialNoteLists.size(); ++idx)
    {
        auto it = incomingNoteSpecial(idx);
        while (!isLastNoteSpecial(idx) && vt >= it->time)
        {
            noteSpecialExpired.push_back(*it);
            it = nextNoteSpecial(idx);
        }
    }

    // update beat
    lunaticvibes::Time currentMeasureTimePassed = vt - _barTimestamp[_currentBarTemp];
    lunaticvibes::Time timeFromBPMChange = currentMeasureTimePassed - _lastChangedBPMTime;
    State::set(IndexNumber::_TEST4, (int)currentMeasureTimePassed.norm());
    _currentMetreTemp = _lastChangedBPMMetre + (double)timeFromBPMChange.hres() / _currentBeatLength.hres() / 4;

    postUpdate(vt);

    State::set(IndexNumber::_TEST1, _currentBarTemp);
    State::set(IndexNumber::_TEST2, (int)std::floor(_currentMetreTemp * 1000));

    _currentBar = _currentBarTemp;
    _currentMetre = _currentMetreTemp;
}
