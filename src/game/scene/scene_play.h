#pragma once
#include "common/chartformat/chartformat.h"
#include "game/chart/chart.h"
#include "game/skin/skin_mgr.h"
#include "scene.h"
#include "scene_context.h"
#include <future>
#include <memory>
#include <mutex>
#include <variant>

enum class ePlayState
{
    PREPARE,
    LOADING,
    LOAD_END,
    PLAYING,
    FAILED,
    FADEOUT,
    WAIT_ARENA,
};

class ScenePlay : public SceneBase
{
public:
    explicit ScenePlay(const std::shared_ptr<SkinMgr>& skinMgr);
    ~ScenePlay() override;

private:
    std::future<void> _loadChartFuture;

private:
    ePlayState state;
    InputMask _inputAvailable;
    std::vector<size_t> keySampleIndex;

protected:
    bool isPlaymodeDP() const;

private:
    bool playInterrupted = false;
    bool playFinished = false;
    bool holdingStart[2]{false, false};
    bool holdingSelect[2]{false, false};
    bool isHoldingStart(int player) const;
    bool isHoldingSelect(int player) const;

    struct PlayerState
    {
        bool finished = false;

        lunaticvibes::Time startPressedTime = TIMER_NEVER;
        lunaticvibes::Time selectPressedTime = TIMER_NEVER;

        double turntableAngleAdd = 0;

        AxisDir scratchDirection = 0;
        lunaticvibes::Time scratchLastUpdate = TIMER_NEVER;
        double scratchAccumulator = 0;

        int hispeedAddPending = 0;
        int lanecoverAddPending = 0;

        double savedHispeed = 1.0;

        Option::e_lane_effect_type origLanecoverType = Option::LANE_OFF;

        int healthLastTick = 0;

        double lockspeedValueInternal = 300.0; // internal use only, for precise calculation
        double lockspeedHispeedBuffered = 2.0;
        int lockspeedGreenNumber = 300; // green number integer

        bool hispeedHasChanged = false;
        bool lanecoverTopHasChanged = false;
        bool lanecoverBottomHasChanged = false;
        bool lanecoverStateHasChanged = false;
        bool lockspeedResetPending = false;

        int judgeBP = 0; // used for displaying poor bga

    } playerState[2];

    lunaticvibes::Time delayedReadyTime = 0;
    unsigned retryRequestTick = 0;

    std::vector<ReplayChart::Commands>::iterator itReplayCommand;
    InputMask replayKeyPressing;
    unsigned replayCmdMapIndex = 0;

    bool isManuallyRequestedExit = false;
    bool isReplayRequestedExit = false;

    lunaticvibes::Time poorBgaStartTime;
    int poorBgaDuration;

    double hiSpeedMinSoft = 0.25;
    double hiSpeedMinHard = 0.01;
    double hiSpeedMax = 10.0;
    double hiSpeedMargin = 0.25;
    int lanecoverMargin = 100;
    bool adjustHispeedWithUpDown = false;
    bool adjustHispeedWithSelect = false;
    bool adjustLanecoverWithStart67 = false;
    bool adjustLanecoverWithMousewheel = false;
    bool adjustLanecoverWithLeftRight = false;

public:
    void clearGlobalDatas();
    bool createChartObj();
    bool createRuleset();

protected:
    void setInitialHealthBMS();

private:
    std::array<size_t, 128> _bgmSampleIdxBuf{};
    std::array<size_t, 128> _keySampleIdxBuf{};

private:
    // std::map<size_t, std::variant<std::monostate, pVideo, pTexture>> _bgaIdxBuf{};
    // std::map<size_t, std::list<std::shared_ptr<SpriteVideo>>> _bgaVideoSprites{};	// set when loading skins, to
    // bind videos while loading chart size_t bgaBaseIdx = -1u; size_t bgaLayerIdx = -1u; size_t bgaPoorIdx = -1u;
    // pTexture bgaBaseTexture;
    // pTexture bgaLayerTexture;
    // pTexture bgaPoorTexture;
public:
    // void bindBgaVideoSprite(size_t idx, std::shared_ptr<SpriteVideo> pv) { _bgaVideoSprites[idx].push_back(pv); }

protected:
    // common
    void loadChart();
    constexpr double getWavLoadProgress()
    {
        return (wavTotal == 0) ? (gChartContext.isSampleLoaded ? 1.0 : 0.0) : (double)wavLoaded / wavTotal;
    }
    constexpr double getBgaLoadProgress()
    {
        return (bmpTotal == 0) ? (gChartContext.isBgaLoaded ? 1.0 : 0.0) : (double)bmpLoaded / bmpTotal;
    }

    void setInputJudgeCallback();
    void removeInputJudgeCallback();

protected:
    // loading indicators
    bool chartObjLoaded = false;
    bool rulesetLoaded = false;
    // bool _sampleLoaded = false;
    // bool _bgaLoaded = false;
    unsigned wavLoaded = 0;
    unsigned wavTotal = 0;
    unsigned bmpLoaded = 0;
    unsigned bmpTotal = 0;

protected:
    // Looper callbacks
    bool readyToStopAsync() const override;
    void _updateAsync() override;
    void updateAsyncLanecover(const lunaticvibes::Time& t);
    void updateAsyncGreenNumber(const lunaticvibes::Time& t);
    void updateAsyncGaugeUpTimer(const lunaticvibes::Time& t);
    void updateAsyncLanecoverDisplay(const lunaticvibes::Time& t);
    void updateAsyncHSGradient(const lunaticvibes::Time& t);
    void updateAsyncAbsoluteAxis(const lunaticvibes::Time& t);
    void updatePrepare();
    void updateLoading();
    void updateLoadEnd();
    void updatePlaying();
    void updateFadeout();
    void updateFailed();
    void updateWaitArena();

protected:
    // Inner-state updates
    void updatePlayTime(const lunaticvibes::Time& rt);
    void procCommonNotes();
    void changeKeySampleMapping(const lunaticvibes::Time& t);
    void spinTurntable(bool startedPlaying);
    void requestExit();
    void toggleLanecover(int slot, bool state);

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, const lunaticvibes::Time&);
    void inputGameHold(InputMask&, const lunaticvibes::Time&);
    void inputGameRelease(InputMask&, const lunaticvibes::Time&);
    void inputGamePressTimer(InputMask&, const lunaticvibes::Time&);
    void inputGamePressPlayKeysounds(InputMask, const lunaticvibes::Time&);
    void inputGameReleaseTimer(InputMask&, const lunaticvibes::Time&);
    void inputGameAxis(double s1, double s2, const lunaticvibes::Time&);

protected:
    bool imguiShowAdjustMenu = false;
    int imguiAdjustBorderX = 640;
    int imguiAdjustBorderY = 480;
    int imguiAdjustBorderSize = 50;
    bool imguiAdjustIsDP = false;
    bool imguiAdjustHas2P = false;

    bool shouldShowImgui() const override;
    void updateImgui() override;
    void imguiInit();
    void imguiAdjustMenu();
};
