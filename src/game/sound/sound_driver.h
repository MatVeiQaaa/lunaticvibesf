#pragma once
#include "common/asynclooper.h"
#include "fmod.hpp"
#include <array>
#include <cstdint>
#include <string>
#include <utility>

using size_t = std::size_t;
using uint8_t = std::uint8_t;

enum class DSPType : uint8_t
{
    OFF,
    REVERB,
    DELAY,
    LOWPASS,
    HIGHPASS,
    FLANGER,
    CHORUS,
    DISTORTION,
};

enum class EQFreq : uint8_t
{
    _62_5,
    _160,
    _400,
    _1000,
    _2500,
    _6250,
    _16k
};

enum class SampleChannel : uint8_t
{
    MASTER,
    KEY,
    BGM,
};

// Exhaustive.
// Iterated on using underlying value.
enum class SoundChannelType : uint8_t
{
    BGM_SYS,
    BGM_NOTE,
    KEY_SYS,
    KEY_LEFT,
    KEY_RIGHT,
    TYPE_COUNT,
};

constexpr int DriverIDUnknownASIO = -10;

class SoundDriver : public AsyncLooper
{
    friend class SoundMgr;

public:
    SoundDriver(std::function<void()> update) : AsyncLooper("SoundDriver", std::move(update), 1000, true) {}
    ~SoundDriver() override = default;

public:
    virtual std::vector<std::pair<int, std::string>> getDeviceList() = 0;
    virtual int setDevice(size_t index) = 0;
    virtual std::pair<int, int> getDSPBufferSize() = 0;

public:
    virtual int loadNoteSample(const Path& path, size_t index) = 0;
    virtual void playNoteSample(SoundChannelType ch, size_t count, size_t index[]) = 0;
    virtual void stopNoteSamples() = 0;
    virtual void freeNoteSamples() = 0;
    virtual long long getNoteSampleLength(size_t index) = 0; // in ms
    virtual int loadSysSample(const Path& path, size_t index, bool isStream = false, bool loop = false) = 0;
    virtual void playSysSample(SoundChannelType ch, size_t index) = 0;
    virtual void stopSysSamples() = 0;
    virtual void freeSysSamples() = 0;

public:
    virtual void setSysVolume(float v, int gradientTime = 0) = 0;
    virtual void setNoteVolume(float v, int gradientTime = 0) = 0;
    virtual void setVolume(SampleChannel ch, float v) = 0;
    virtual void setDSP(DSPType type, int index, SampleChannel ch, float p1, float p2) = 0;
    virtual void setFreqFactor(double f) = 0;
    virtual void setSpeed(double speed) = 0;
    virtual void setPitch(double pitch) = 0;
    virtual void setEQ(EQFreq freq, int gain) = 0;
};
