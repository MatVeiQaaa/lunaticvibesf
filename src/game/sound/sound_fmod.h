#pragma once

#include <array>
#include <string>
#include <thread>
#include "sound_driver.h"
#include "fmod.hpp"
#include "common/types.h"

// This game uses FMOD Low Level API to play sounds as we don't use FMOD Studio,

class SoundDriverFMOD: public SoundDriver
{
    friend class SoundMgr;

private:
	FMOD::System *fmodSystem = nullptr;
	int initRet;

protected:
	std::map<SoundChannelType, std::shared_ptr<FMOD::ChannelGroup>> channelGroup;
	std::map<SampleChannel, float> volume;
	float sysVolume = 1.0;

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
	std::array<SoundSample, NOTESAMPLES> noteSamples{};  // Sound samples of key sound
	std::array<SoundSample, SYSSAMPLES> sysSamples{};  // Sound samples of BGM, effect, etc

public:
	SoundDriverFMOD();
	virtual ~SoundDriverFMOD();
	void createChannelGroups();

public:
	virtual std::vector<std::pair<int, std::string>> getDeviceList(bool asio = false);
	virtual int setDevice(size_t index, bool asio = false);
	int findDriver(const std::string& name, bool asio);

private:
    bool bLoading = false;
    std::thread tLoadSampleThread;
    void loadSampleThread();

public:
    int setAsyncIO(bool async = true);

public:
	virtual int loadNoteSample(const Path& path, size_t index);
	virtual void playNoteSample(SoundChannelType ch, size_t count, size_t index[]);
	virtual void stopNoteSamples();
	virtual void freeNoteSamples();
	virtual long long getNoteSampleLength(size_t index);
	virtual void update();

public:
	virtual int loadSysSample(const Path& path, size_t index, bool isStream = false, bool loop = false);
	virtual void playSysSample(SoundChannelType ch, size_t index);
	virtual void stopSysSamples();
	virtual void freeSysSamples();
	int getChannelsPlaying();

public:
	virtual void setSysVolume(float v);
	virtual void setVolume(SampleChannel ch, float v);
	virtual void setDSP(DSPType type, int index, SampleChannel ch, float p1, float p2);
	virtual void setFreqFactor(double f);
	virtual void setSpeed(double speed);
	virtual void setPitch(double pitch);
	virtual void setEQ(EQFreq freq, int gain);

};
