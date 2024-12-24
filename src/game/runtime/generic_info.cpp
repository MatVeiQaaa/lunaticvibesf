#include "generic_info.h"

#include <common/asynclooper.h>
#include <common/sysutil.h>
#include <game/runtime/state.h>

#include <chrono>
#include <ctime>

GenericInfoUpdater::GenericInfoUpdater(unsigned rate)
    : AsyncLooper("GenericInfoUpdater", std::bind_front(&GenericInfoUpdater::loop, this), rate)
{
}

void GenericInfoUpdater::loop()
{
    State::set(IndexNumber::FPS, gFrameCount[FRAMECOUNT_IDX_FPS] / _rate);
    gFrameCount[FRAMECOUNT_IDX_FPS] = 0;

    for (unsigned i = 1; i < std::size(gFrameCount); ++i)
    {
        State::set((IndexNumber)((int)IndexNumber::_PPS1 + i - 1), gFrameCount[i] / _rate);
        gFrameCount[i] = 0;
    }
    State::set(IndexNumber::SCENE_UPDATE_FPS, State::get(IndexNumber::_PPS1));
    State::set(IndexNumber::INPUT_DETECT_FPS, State::get(IndexNumber::_PPS2));

    std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm tt;
    auto d = lunaticvibes::safe_localtime(&t, &tt);
    if (d)
    {
        State::set(IndexNumber::DATE_YEAR, d->tm_year + 1900);
        State::set(IndexNumber::DATE_MON, d->tm_mon + 1);
        State::set(IndexNumber::DATE_DAY, d->tm_mday);
        State::set(IndexNumber::DATE_HOUR, d->tm_hour);
        State::set(IndexNumber::DATE_MIN, d->tm_min);
        State::set(IndexNumber::DATE_SEC, d->tm_sec);
    }
}
