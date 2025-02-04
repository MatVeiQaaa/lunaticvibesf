#pragma once

#include "common/beat.h"
#include "common/types.h"
#include "fmod.hpp"
#include "sound_driver.h"
#include <array>
#include <string>

// This game uses FMOD Low Level API to play sounds as we don't use FMOD Studio,

class SoundDriverFMOD : public SoundDriver
{
    friend class SoundMgr;

private:
    FMOD::System* fmodSystem = nullptr;
    int initRet;

protected:
    std::map<SoundChannelType, std::shared_ptr<FMOD::ChannelGroup>> channelGroup;
    std::map<SampleChannel, float> volume;
    float sysVolume = 1.0;
    float noteVolume = 1.0;

    float sysVolumeGradientBegin = 1.0;
    float sysVolumeGradientEnd = 1.0;
    lunaticvibes::Time sysVolumeGradientBeginTime;
    int sysVolumeGradientLength = 0;
    float noteVolumeGradientBegin = 1.0;
    float noteVolumeGradientEnd = 1.0;
    lunaticvibes::Time noteVolumeGradientBeginTime;
    int noteVolumeGradientLength = 0;

    std::map<SoundChannelType, FMOD::DSP*> DSPMaster[3];
    std::map<SoundChannelType, FMOD::DSP*> DSPKey[3];
    std::map<SoundChannelType, FMOD::DSP*> DSPBgm[3];
    std::map<SoundChannelType, FMOD::DSP*> PitchShiftFilter;
    std::map<SoundChannelType, FMOD::DSP*> EQFilter[2];

public:
    static constexpr size_t NOTESAMPLES = 36 * 36 + 1;
    static constexpr size_t SYSSAMPLES = 64;
    struct SoundSample
    {
        FMOD::Sound* objptr = nullptr;
        std::string path;
        int flags = 0;
    };

protected:
    std::array<SoundSample, NOTESAMPLES> noteSamples{}; // Sound samples of key sound
    std::array<SoundSample, SYSSAMPLES> sysSamples{};   // Sound samples of BGM, effect, etc

public:
    SoundDriverFMOD();
    ~SoundDriverFMOD() override;
    void createChannelGroups();

public:
    std::vector<std::pair<int, std::string>> getDeviceList() override;
    int setDevice(size_t index) override;
    std::pair<int, int> getDSPBufferSize() override;

private:
    int findDriver(const std::string& name, int driverIDUnknown);

public:
    int setAsyncIO(bool async = true);

public:
    int loadNoteSample(const Path& path, size_t index) override;
    void playNoteSample(SoundChannelType ch, size_t count, size_t index[]) override;
    void stopNoteSamples() override;
    void freeNoteSamples() override;
    long long getNoteSampleLength(size_t index) override;
    virtual void update();

public:
    int loadSysSample(const Path& path, size_t index, bool isStream = false, bool loop = false) override;
    void playSysSample(SoundChannelType ch, size_t index) override;
    void stopSysSamples() override;
    void freeSysSamples() override;
    int getChannelsPlaying();

public:
    void setSysVolume(float v, int gradientTime = 0) override;
    void setNoteVolume(float v, int gradientTime = 0) override;
    void setVolume(SampleChannel ch, float v) override;
    void setDSP(DSPType type, int index, SampleChannel ch, float p1, float p2) override;
    void setFreqFactor(double f) override;
    void setSpeed(double speed) override;
    void setPitch(double pitch) override;
    void setEQ(EQFreq freq, int gain) override;
};
