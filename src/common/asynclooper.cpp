#include "asynclooper.h"
#include <chrono>

#ifdef _WIN32
#include "encoding.h"
#else
#include <ratio>
#include <utility>
#endif

#include "log.h"
#include <common/assert.h>
#include <common/sysutil.h>
#include <common/utils.h>

AsyncLooper::AsyncLooper(StringContentView tag, std::function<void()> func, unsigned rate_per_sec, bool single_inst)
    : _tag(tag), _loopFunc(std::move(func))
{
    _rate = rate_per_sec;

#ifdef _WIN32
    handler = CreateWaitableTimerExA(NULL, NULL, 0, TIMER_ALL_ACCESS);
    LVF_DEBUG_ASSERT(handler != NULL);
#endif
}

AsyncLooper::~AsyncLooper()
{
    LVF_DEBUG_ASSERT(!_running);

#ifdef _WIN32
    if (handler)
    {
        CloseHandle(handler);
        handler = NULL;
    }
#endif
}

void AsyncLooper::setRate(unsigned rate_per_sec)
{
    _rate = rate_per_sec;
    if (_running)
    {
        loopEnd();
        loopStart();
    }
}

void AsyncLooper::run()
{
#ifndef NDEBUG
    _runThreadID = GetCurrentThreadID();
#endif
    if (_running && !_inLoopBody)
    {
        _inLoopBody = true;
        _loopFunc();
        _inLoopBody = false;
    }
}

unsigned AsyncLooper::getRate()
{
    return _rate;
}

#ifdef _WIN32

void AsyncLooper::loopStart()
{
    if (!_running)
    {
        if (handler)
        {
            _running = true;

            loopFuture = std::async(std::launch::async, [this]() {
                SetThreadName(_tag.c_str());
                long long us = _rate > 0 ? 1000000 / _rate : 0;
                long long reset_threshold = us * 4;

                LARGE_INTEGER dueTime = {0};
                dueTime.QuadPart = -(static_cast<long long>(us) * 10);

                using namespace std::chrono;
                using namespace std::chrono_literals;

                tStart = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();

                try
                {
                    while (_running)
                    {
                        if (us > 0 && dueTime.QuadPart < 0)
                        {
                            SetWaitableTimerEx(handler, &dueTime, 0, NULL, NULL, NULL, 0);
                            // SleepEx(100, TRUE);
                            WaitForSingleObjectEx(handler, 1000, TRUE);
                        }

                        run();

                        auto t2 = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();

                        if (us > (t2 - tStart))
                        {
                            dueTime.QuadPart = -(static_cast<long long>(us - (t2 - tStart)) * 10);
                        }
                        else
                        {
                            dueTime.QuadPart = 0;
                        }

                        if ((t2 - tStart) < reset_threshold)
                        {
                            tStart += us;
                        }
                        else
                        {
                            tStart = t2;
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    LOG_ERROR << "[AsyncLooper] " << _tag
                              << ": Exception: " << to_utf8(e.what(), eFileEncoding::LATIN1);
                    throw;
                }
            });
        }

        if (_running)
        {
            LOG_DEBUG << "[Looper] " << _tag << ": Started " << _rate << "/s";
        }
    }
}

void AsyncLooper::loopEnd()
{
    if (_running)
    {
        _running = false;
        loopFuture.wait_for(std::chrono::seconds(1));
        if (CancelWaitableTimer(handler) == 0)
        {
            DWORD dwError = GetLastError();
            if (dwError != ERROR_INVALID_HANDLE)
            {
                LOG_ERROR << "[Looper] " << _tag << ": Delete timer error: " << dwError;
                while (dwError == ERROR_IO_PENDING && DeleteTimerQueueTimer(NULL, handler, INVALID_HANDLE_VALUE) != 0)
                {
                    dwError = GetLastError();
                }
                LVF_DEBUG_ASSERT(dwError == 0);
            }
        }
        LOG_DEBUG << "[Looper] " << _tag << ": Ended " << _rate << "/s";
    }
}

#else // FALLBACK
void AsyncLooper::_loopWithSleep()
{
    SetThreadName(_tag.c_str());
    LOG_DEBUG << "[Looper] " << _tag << ": Starting " << _rate << "/s";
    std::chrono::high_resolution_clock::time_point frameTimestampPrev;
    const auto desiredFrameTimeBetweenFrames = std::chrono::nanoseconds(std::nano::den / _rate);
    while (_running)
    {
        run();
        const long long sleep_duration =
            std::chrono::duration_cast<std::chrono::nanoseconds>(frameTimestampPrev + desiredFrameTimeBetweenFrames -
                                                                 std::chrono::high_resolution_clock::now())
                .count();
        preciseSleep(sleep_duration);
        frameTimestampPrev = std::chrono::high_resolution_clock::now();
    }
    LOG_DEBUG << "[Looper] " << _tag << ": End " << _rate << "/s";
}

void AsyncLooper::loopStart()
{
    if (_running)
        return;
    _running = true;
    handler = std::thread(&AsyncLooper::_loopWithSleep, this);
}

void AsyncLooper::loopEnd()
{
    if (!_running)
        return;
    _running = false;
    handler.join();
}
#endif
