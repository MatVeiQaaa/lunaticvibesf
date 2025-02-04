#pragma once

#include "sprite.h"

////////////////////////////////////////////////////////////////////////////////
// Line sprite
struct ColorPoint
{
    double xf, yf;
    Color c;
};

// Private enum, LVF internal.
enum class LineType
{
    GAUGE_F,
    GAUGE_C,
    SCORE,
    SCORE_MYBEST,
    SCORE_TARGET,
};

class SpriteLine : public SpriteStatic
{
private:
    int _player;
    LineType _ltype;
    Color textColor;
    int _field_w, _field_h;
    int _start, _end;

    GraphLine _line;
    std::vector<ColorPoint> _points;
    std::vector<std::pair<Point, Point>> _rects;
    double _progress = 1.0; // 0 ~ 1

public:
    struct SpriteLineBuilder : SpriteStaticBuilder
    {
        int player = 0;
        LineType lineType = LineType::GAUGE_F;
        int canvasW = 0;
        int canvasH = 0;
        int start = 0;
        int end = 0;
        int lineWeight = 0;
        Color color = 0xffffffff;

        std::shared_ptr<SpriteLine> build() { return std::make_shared<SpriteLine>(*this); }
    };

public:
    SpriteLine() = delete;
    SpriteLine(const SpriteLineBuilder& builder);
    ~SpriteLine() override = default;

public:
    void appendPoint(const ColorPoint&);

public:
    void updateProgress(const lunaticvibes::Time& t);
    void updateRects();
    bool update(const lunaticvibes::Time& t) override;
    void draw() const override;
};
