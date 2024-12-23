#pragma once

#include <common/asynclooper.h>

#include <atomic>
#include <ctime>

inline std::atomic<unsigned> gFrameCount[3]{0};
constexpr size_t FRAMECOUNT_IDX_FPS = 0;
constexpr size_t FRAMECOUNT_IDX_SCENE = 1;
constexpr size_t FRAMECOUNT_IDX_INPUT = 2;

class GenericInfoUpdater : public AsyncLooper
{
public:
    GenericInfoUpdater(unsigned rate);

private:
    void loop();
};
