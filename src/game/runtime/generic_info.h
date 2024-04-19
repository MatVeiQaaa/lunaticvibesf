#pragma once
#include <chrono>
#include <ctime>
#include "state.h"
#include "common/asynclooper.h"
#include "common/sysutil.h"

inline unsigned gFrameCount[10]{ 0 };
constexpr size_t FRAMECOUNT_IDX_FPS = 0;
constexpr size_t FRAMECOUNT_IDX_SCENE = 1;
constexpr size_t FRAMECOUNT_IDX_INPUT = 2;

// Should only have one instance at once.
class GenericInfoUpdater : public AsyncLooper
{
public:
	GenericInfoUpdater(unsigned rate) : AsyncLooper("GenericInfoUpdater", std::bind(&GenericInfoUpdater::_loop, this), rate) {}
private:
	void _loop()
	{
		State::set(IndexNumber::FPS, gFrameCount[0] / _rate);
		gFrameCount[FRAMECOUNT_IDX_FPS] = 0;

		for (unsigned i = 1; i < 10; ++i)
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

		//createNotification(std::to_string(t));
	}
};
