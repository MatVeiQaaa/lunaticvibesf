#pragma once
#include <array>
#include <vector>
#include <list>
#include <utility>
#include <chrono>
#include "beat.h"
#include "game/data/timer.h"
#include "game/input/input_mgr.h"

struct sNote: Note
{
    bool hit = false;
};

enum class NoteChannelCategory: size_t
{
    Note,
    Mine,
    Invs,
    LN,
    _, // INVALID
    NOTECATEGORY_COUNT,
};

enum NoteChannelIndex: size_t
{
    Sc1 = 0,
    K1,
    K2,
    K3,
    K4,
    K5,
    K6,
    K7,
    K8,
    K9,
    K10,
    K11,
    K12,
    K13,
    K14,
    K15,
    K16,
    K17,
    K18,
    K19,
    K20,
    K21,
    K22,
    K23,
    K24,
    Sc2,

    NOTECHANNEL_COUNT,
    _ // INVALID
};

constexpr size_t channelToIdx(NoteChannelCategory cat, NoteChannelIndex idx)
{
    return (cat != NoteChannelCategory::_) ? 
        (size_t)cat * NOTECHANNEL_COUNT + idx : 
        0, NoteChannelIndex::_;
}

// Chart in-game data representation. Contains following:
//  - Converts plain-beat to real-beat (adds up Stop beat) 
//  - Converts time to beat (if necessary)
//  - Converts (or reads directly) beat to time
//  - Measure Time Indicator
//  - Segmented Scrolling speed (power, 0 at stop)
// [Input]  Current Time(hTime)
// [Output] Current Measure(unsigned) 
//          Beat(double)
//          BPM(BPM)
//          Expired Notes List (including normal, plain, ext, speed)
class vScroll
{
public:
    static const size_t MAX_MEASURES = 1000;
    static const size_t CHANNEL_COUNT = (size_t)NoteChannelCategory::NOTECATEGORY_COUNT * NOTECHANNEL_COUNT;

protected:

     // full list of corresponding channel through all measures; only this list is handled by input looper
    std::array<std::list<sNote>, CHANNEL_COUNT> _noteLists;

    std::vector<std::list<Note>> _plainLists;     // basically sNote without hit param, including BGM, BGA; handled with timer
    std::vector<std::list<Note>> _extLists;       // e.g. Stop in BMS
    std::list<Note>              _bpmList;
    std::list<Note>              _scrollingSpeedList;

    //std::array<Beat,   MAX_MEASURES> _measureLength;
    std::array<hTime, MAX_MEASURES>   _measureTimestamp;


public:
    vScroll() = delete;
    vScroll(size_t plain_n, size_t ext_n);

private:
    std::array<decltype(_noteLists.front().begin()), CHANNEL_COUNT> _noteListIterators;
    std::vector<decltype(_plainLists.front().begin())>  _plainListIterators;
    std::vector<decltype(_extLists.front().begin())>    _extListIterators;
    decltype(_bpmList.begin())             _bpmListIterator;
    decltype(_scrollingSpeedList.begin())  _speedListIterator;
public:
    auto lastNoteOfChannel      (NoteChannelCategory cat, NoteChannelIndex idx) -> decltype(_noteListIterators.front());
    auto lastNoteOfPlainChannel (size_t idx) -> decltype(_plainListIterators.front());
    auto lastNoteOfExtChannel   (size_t idx) -> decltype(_extListIterators.front());
    auto lastNoteOfBpm          () -> decltype(_bpmListIterator);
    auto lastNoteOfSpeed        () -> decltype(_speedListIterator);
    bool isLastNoteOfChannel      (NoteChannelCategory cat, NoteChannelIndex idx);
    bool isLastNoteOfPlainChannel (size_t idx);
    bool isLastNoteOfExtChannel   (size_t idx);
    bool isLastNoteOfBpm();
    bool isLastNoteOfSpeed();
    auto succNoteOfChannel      (NoteChannelCategory cat, NoteChannelIndex idx) -> decltype(_noteListIterators.front());
    auto succNoteOfPlainChannel (size_t idx) -> decltype(_plainListIterators.front());
    auto succNoteOfExtChannel   (size_t idx) -> decltype(_extListIterators.front());
    auto succNoteOfBpm          () -> decltype(_bpmListIterator);
    auto succNoteOfSpeed        () -> decltype(_speedListIterator);

public:
    hTime getMeasureLength(size_t measure);
    hTime getCurrentMeasureLength();

protected:
    unsigned _currentMeasure    = 0;
    double   _currentBeat       = 0;
    double   _currentRenderBeat = 0;
    BPM      _currentBPM        = 150.0;
    double   _currentSpeed      = 1.0;
    hTime    _lastChangedTime   = 0;
    double   _lastChangedBeat   = 0;
    hTime    _lastChangedSpeedTime = 0;

public:
    void reset();
    void setIterators();            // set after parsing
    /*virtual*/ void update(hTime t);
    constexpr auto getCurrentMeasure() -> decltype(_currentMeasure) { return _currentMeasure; }
    constexpr auto getCurrentBeat() -> decltype(_currentBeat) { return _currentBeat; }
    constexpr auto getCurrentRenderBeat() -> decltype(_currentRenderBeat) { return _currentRenderBeat; }
    constexpr auto getCurrentBPM() -> decltype(_currentBPM) { return _currentBPM; }

public:
    std::list<sNote>  noteExpired;
    std::list<Note>   notePlainExpired;
    std::list<Note>   noteExtExpired;

public:
    virtual std::pair<NoteChannelCategory, NoteChannelIndex> getChannelFromKey(Input::Ingame input) = 0;
    virtual std::vector<Input::Ingame> getInputFromChannel(size_t channel) = 0;
};
