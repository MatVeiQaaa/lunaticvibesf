#include "scene_result.h"
#include "scene_context.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

SceneResult::SceneResult(ePlayMode gamemode) : vScene(eMode::RESULT, 1000), _mode(gamemode)
{
    _inputAvailable = INPUT_MASK_FUNC;

    if (gPlayContext.chartObj[PLAYER_SLOT_1P] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_1P;
    }
        
    if (gPlayContext.chartObj[PLAYER_SLOT_2P] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_2P;
    }

    _state = eResultState::DRAW;

    // set options
    if (gPlayContext.ruleset[PLAYER_SLOT_1P])
    {
        auto d1p = gPlayContext.ruleset[PLAYER_SLOT_1P]->getData();

        if (d1p.total_acc >= 100.0)      gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_0);
        else if (d1p.total_acc >= 88.88) gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_1);
        else if (d1p.total_acc >= 77.77) gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_2);
        else if (d1p.total_acc >= 66.66) gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_3);
        else if (d1p.total_acc >= 55.55) gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_4);
        else if (d1p.total_acc >= 44.44) gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_5);
        else if (d1p.total_acc >= 33.33) gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_6);
        else if (d1p.total_acc >= 22.22) gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_7);
        else                             gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_8);
    }

    if (gPlayContext.ruleset[PLAYER_SLOT_2P])
    {
        auto d2p = gPlayContext.ruleset[PLAYER_SLOT_2P]->getData();
        if (d2p.total_acc >= 100.0)      gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_0);
        else if (d2p.total_acc >= 88.88) gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_1);
        else if (d2p.total_acc >= 77.77) gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_2);
        else if (d2p.total_acc >= 66.66) gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_3);
        else if (d2p.total_acc >= 55.55) gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_4);
        else if (d2p.total_acc >= 44.44) gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_5);
        else if (d2p.total_acc >= 33.33) gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_6);
        else if (d2p.total_acc >= 22.22) gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_7);
        else                             gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_8);
    }

    gOptions.flush();

    // TODO compare to db record
    auto dp = gPlayContext.ruleset[PLAYER_SLOT_1P]->getData();

    bool cleared = gSwitches.get(eSwitch::RESULT_CLEAR);

    switch (gPlayContext.mode)
    {
    case eMode::PLAY5_2:
    case eMode::PLAY7_2:
    case eMode::PLAY9_2:
    {
        auto d1p = gPlayContext.ruleset[PLAYER_SLOT_1P]->getData();
        auto d2p = gPlayContext.ruleset[PLAYER_SLOT_2P]->getData();

        // TODO WIN/LOSE
        /*
        switch (context_play.rulesetType)
        {
        case eRuleset::CLASSIC:
            if (d1p.score2 > d2p.score2)
                // TODO
                break;

        default:
            if (d1p.score > d2p.score)
                break;
        }
        */

        // clear or failed?
        //cleared = gPlayContext.ruleset[PLAYER_SLOT_1P]->isCleared() || gPlayContext.ruleset[PLAYER_SLOT_2P]->isCleared();
        break;
    }

    default:
        //cleared = gPlayContext.ruleset[PLAYER_SLOT_1P]->isCleared();
        break;
    }

    // Moved to play
    //gSwitches.set(eSwitch::RESULT_CLEAR, cleared);

    if (_mode != ePlayMode::LOCAL_BATTLE && !gChartContext.hash.empty())
    {
        _pScoreOld = g_pScoreDB->getChartScoreBMS(gChartContext.hash);
    }

    using namespace std::placeholders;
    _input.register_p("SCENE_PRESS", std::bind(&SceneResult::inputGamePress, this, _1, _2));
    _input.register_h("SCENE_HOLD", std::bind(&SceneResult::inputGameHold, this, _1, _2));
    _input.register_r("SCENE_RELEASE", std::bind(&SceneResult::inputGameRelease, this, _1, _2));

    Time t;
    gTimers.set(eTimer::RESULT_GRAPH_START, t.norm());

    loopStart();
    _input.loopStart();

    SoundMgr::stopSamples();
    if (cleared) 
        SoundMgr::playSample(eSoundSample::SOUND_CLEAR);
    else
        SoundMgr::playSample(eSoundSample::SOUND_FAIL);
}

////////////////////////////////////////////////////////////////////////////////

void SceneResult::_updateAsync()
{
    std::unique_lock<decltype(_mutex)> _lock(_mutex, std::try_to_lock);
    if (!_lock.owns_lock()) return;

    switch (_state)
    {
    case eResultState::DRAW:
        updateDraw();
        break;
    case eResultState::STOP:
        updateStop();
        break;
    case eResultState::RECORD:
        updateRecord();
        break;
    case eResultState::FADEOUT:
        updateFadeout();
        break;
    }
}

void SceneResult::updateDraw()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);

    if (rt.norm() >= _skin->info.timeResultRank)
    {
        gTimers.set(eTimer::RESULT_RANK_START, t.norm());
        // TODO play hit sound
        _state = eResultState::STOP;
        LOG_DEBUG << "[Result] State changed to STOP";
    }
}

void SceneResult::updateStop()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneResult::updateRecord()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneResult::updateFadeout()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
    auto ft = t - gTimers.get(eTimer::FADEOUT_BEGIN);

    if (ft >= _skin->info.timeOutro)
    {
        loopEnd();
        _input.loopEnd();
        SoundMgr::stopKeySamples();

        // save score
        if (_mode != ePlayMode::LOCAL_BATTLE && !gChartContext.hash.empty())
        {
            assert(gPlayContext.ruleset[PLAYER_SLOT_1P] != nullptr);
            ScoreBMS score;
            auto& format = gChartContext.chartObj;
            auto& chart = gPlayContext.chartObj[PLAYER_SLOT_1P];
            auto& ruleset = gPlayContext.ruleset[PLAYER_SLOT_1P];
            auto& data = ruleset->getData();
            score.notes = chart->getNoteCount();
            score.score = data.score;
            score.rate = data.total_acc;
            score.fast = data.fast;
            score.slow = data.slow;
            score.maxcombo = data.maxCombo;
            score.playcount = _pScoreOld ? _pScoreOld->playcount + 1 : 1;
            switch (format->type())
            {
            case eChartFormat::BMS:
            case eChartFormat::BMSON:
            {
                auto rBMS = std::reinterpret_pointer_cast<RulesetBMS>(ruleset);
                score.exscore = data.score2;

                score.lamp = ScoreBMS::Lamp::NOPLAY;
                if (rBMS->isCleared())
                {
                    if (gPlayContext.isCourse)
                    {
                        if (rBMS->getMaxCombo() == rBMS->getData().maxCombo)
                            score.lamp = ScoreBMS::Lamp::FULLCOMBO;
                    }
                    else
                    {
                        switch (rBMS->getGaugeType())
                        {
                        case RulesetBMS::GaugeType::GROOVE:  score.lamp = ScoreBMS::Lamp::NORMAL; break;
                        case RulesetBMS::GaugeType::EASY:    score.lamp = ScoreBMS::Lamp::EASY; break;
                        case RulesetBMS::GaugeType::ASSIST:  score.lamp = ScoreBMS::Lamp::ASSIST; break;
                        case RulesetBMS::GaugeType::HARD:    score.lamp = ScoreBMS::Lamp::HARD; break;
                        case RulesetBMS::GaugeType::EXHARD:  score.lamp = ScoreBMS::Lamp::EXHARD; break;
                        case RulesetBMS::GaugeType::DEATH:   score.lamp = ScoreBMS::Lamp::FULLCOMBO; break;
                        case RulesetBMS::GaugeType::P_ATK:   score.lamp = ScoreBMS::Lamp::EASY; break;
                        case RulesetBMS::GaugeType::G_ATK:   score.lamp = ScoreBMS::Lamp::EASY; break;
                        case RulesetBMS::GaugeType::GRADE:   score.lamp = ScoreBMS::Lamp::NOPLAY; break;
                        case RulesetBMS::GaugeType::EXGRADE: score.lamp = ScoreBMS::Lamp::NOPLAY; break;
                        default: break;
                        }
                    }
                }
                else
                {
                    score.lamp = ScoreBMS::Lamp::FAILED;
                }

                score.pgreat = rBMS->getJudgeCount(RulesetBMS::JudgeType::PERFECT);
                score.great = rBMS->getJudgeCount(RulesetBMS::JudgeType::GREAT);
                score.good = rBMS->getJudgeCount(RulesetBMS::JudgeType::GOOD);
                score.bad = rBMS->getJudgeCount(RulesetBMS::JudgeType::BAD);
                score.bpoor = rBMS->getJudgeCount(RulesetBMS::JudgeType::BPOOR);
                score.miss = rBMS->getJudgeCount(RulesetBMS::JudgeType::MISS);
                score.bp = score.bad + score.bpoor + score.miss;
                score.combobreak = rBMS->getJudgeCount(RulesetBMS::JudgeType::COMBOBREAK);
                g_pScoreDB->updateChartScoreBMS(gChartContext.hash, score);
                break;
            }
            default:
                break;
            }
        }

        // check retry
        if (_retryRequested && gPlayContext.canRetry)
        {
            clearContextPlayForRetry();
            gNextScene = eScene::PLAY;
        }
        else
        {
            clearContextPlay();
            gNextScene = gQuitOnFinish ? eScene::EXIT : eScene::SELECT;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneResult::inputGamePress(InputMask& m, const Time& t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;

    if ((_inputAvailable & m & INPUT_MASK_DECIDE).any())
    {
        switch (_state)
        {
        case eResultState::DRAW:
            gTimers.set(eTimer::RESULT_RANK_START, t.norm());
            // TODO play hit sound
            _state = eResultState::STOP;
            LOG_DEBUG << "[Result] State changed to STOP";
            break;

        case eResultState::STOP:
            gTimers.set(eTimer::RESULT_HIGHSCORE_START, t.norm());
            // TODO stop result sound
            // TODO play record sound
            _state = eResultState::RECORD;
            LOG_DEBUG << "[Result] State changed to RECORD";
            break;

        case eResultState::RECORD:
            if (_scoreSyncFinished || true) // debug
            {
                gTimers.set(eTimer::FADEOUT_BEGIN, t.norm());
                _state = eResultState::FADEOUT;
                LOG_DEBUG << "[Result] State changed to FADEOUT";
            }
            break;

        case eResultState::FADEOUT:
            break;

        default:
            break;
        }
    }
}

// CALLBACK
void SceneResult::inputGameHold(InputMask& m, const Time& t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;

    if (_state == eResultState::FADEOUT)
    {
        _retryRequested =
            (_inputAvailable & m & INPUT_MASK_DECIDE).any() && 
            (_inputAvailable & m & INPUT_MASK_CANCEL).any();
    }
}

// CALLBACK
void SceneResult::inputGameRelease(InputMask& m, const Time& t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;
}
