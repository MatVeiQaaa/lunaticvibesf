#include "ruleset_bms_replay.h"
#include "game/scene/scene.h"
#include "game/scene/scene_context.h"

#include <cassert>
#include <optional>
#include <utility>

RulesetBMSReplay::RulesetBMSReplay(
    std::shared_ptr<ChartFormatBase> format_,
    std::shared_ptr<ChartObjectBase> chart_,
    std::shared_ptr<ReplayChart> replay_,
    PlayModifierGaugeType gauge,
    GameModeKeys keys,
    JudgeDifficulty difficulty,
    double health,
    PlaySide side)
    : RulesetBase(format_, chart_)
    , RulesetBMS(std::move(format_), std::move(chart_), gauge, keys, difficulty, health, side)
    , replay(std::move(replay_))
{
    itReplayCommand = replay->commands.begin();
    showJudge = (_side == PlaySide::AUTO || _side == PlaySide::AUTO_DOUBLE || _side == PlaySide::AUTO_2P);

    if (gPlayContext.mode == SkinType::PLAY5 || gPlayContext.mode == SkinType::PLAY5_2)
    {
        if (gPlayContext.shift1PNotes5KFor7KSkin)
        {
            replayCmdMapIndex = gPlayContext.shift2PNotes5KFor7KSkin ? 3 : 2;
        }
        else
        {
            replayCmdMapIndex = gPlayContext.shift2PNotes5KFor7KSkin ? 1 : 0;
        }
    }

    if (replay->pitchValue != 0)
    {
        static const double tick = std::pow(2, 1.0 / 12);
        playbackSpeed = std::pow(tick, replay->pitchValue);
    }

    switch (side)
    {
    case RulesetBMS::PlaySide::AUTO:
    case RulesetBMS::PlaySide::AUTO_DOUBLE:
        _judgeScratch = !(gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
        break;

    case RulesetBMS::PlaySide::AUTO_2P:
        _judgeScratch = !(gPlayContext.mods[PLAYER_SLOT_TARGET].assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
        break;

    case RulesetBMS::PlaySide::MYBEST:
        _judgeScratch = !(gPlayContext.mods[PLAYER_SLOT_MYBEST].assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
        break;

    case RulesetBMS::PlaySide::SINGLE:
    case RulesetBMS::PlaySide::DOUBLE:
    case RulesetBMS::PlaySide::BATTLE_1P:
    case RulesetBMS::PlaySide::BATTLE_2P:
    case RulesetBMS::PlaySide::RIVAL:
    case RulesetBMS::PlaySide::NETWORK:
        assert(false);
        break;
    }
}

void RulesetBMSReplay::update(const lunaticvibes::Time& t)
{
    const bool skipToEnd = t.hres() == LLONG_MAX;

    if (!_hasStartTime)
    {
        setStartTime(skipToEnd ? lunaticvibes::Time(0) : t);
    }

    auto rt = t - _startTime.norm();
    _basic.play_time = rt;
    using namespace chart;

    const InputMask prevKeyPressing = keyPressing;
    while (itReplayCommand != replay->commands.end() && rt.norm() >= (long long)std::round(itReplayCommand->ms * playbackSpeed / gSelectContext.pitchSpeed))
    {
        auto cmd = itReplayCommand->type;

        if (_side == PlaySide::AUTO_2P)
        {
            cmd = ReplayChart::Commands::leftSideCmdToRightSide(cmd);
        }

        if (!skipToEnd)
        {
            if (gPlayContext.mode == SkinType::PLAY5 || gPlayContext.mode == SkinType::PLAY5_2)
            {
                if (auto it = REPLAY_CMD_INPUT_DOWN_MAP_5K[replayCmdMapIndex].find(cmd);
                    it != REPLAY_CMD_INPUT_DOWN_MAP_5K[replayCmdMapIndex].end())
                {
                    keyPressing[it->second] = true;
                }
                else if (REPLAY_CMD_INPUT_UP_MAP_5K[replayCmdMapIndex].find(cmd) != REPLAY_CMD_INPUT_UP_MAP_5K[replayCmdMapIndex].end())
                {
                    keyPressing[REPLAY_CMD_INPUT_UP_MAP_5K[replayCmdMapIndex].at(cmd)] = false;
                }
            }
            else
            {
                if (auto it = REPLAY_CMD_INPUT_DOWN_MAP.find(cmd); it != REPLAY_CMD_INPUT_DOWN_MAP.end())
                {
                    keyPressing[it->second] = true;
                }
                else if (REPLAY_CMD_INPUT_UP_MAP.find(cmd) != REPLAY_CMD_INPUT_UP_MAP.end())
                {
                    keyPressing[REPLAY_CMD_INPUT_UP_MAP.at(cmd)] = false;
                }
            }

            switch (cmd)
            {
            case ReplayChart::Commands::Type::S1A_PLUS:  playerScratchAccumulator[PLAYER_SLOT_PLAYER] = 0.0015; break;
            case ReplayChart::Commands::Type::S1A_MINUS: playerScratchAccumulator[PLAYER_SLOT_PLAYER] = -0.0015; break;
            case ReplayChart::Commands::Type::S1A_STOP:  playerScratchAccumulator[PLAYER_SLOT_PLAYER] = 0; break;
            case ReplayChart::Commands::Type::S2A_PLUS:  playerScratchAccumulator[PLAYER_SLOT_TARGET] = 0.0015; break;
            case ReplayChart::Commands::Type::S2A_MINUS: playerScratchAccumulator[PLAYER_SLOT_TARGET] = -0.0015; break;
            case ReplayChart::Commands::Type::S2A_STOP:  playerScratchAccumulator[PLAYER_SLOT_TARGET] = 0; break;

        default:
            break;
            }
        }

        itReplayCommand++;
    }

    InputMask pressed, released;
    if (!skipToEnd)
    {
        pressed = keyPressing & ~prevKeyPressing;
        released = ~keyPressing & prevKeyPressing;
    }
    else
    {
        released = keyPressing;
    }
    if (pressed.any())
        RulesetBMS::updatePress(pressed, t);
    if (keyPressing.any())
        RulesetBMS::updateHold(keyPressing, t);
    if (released.any())
        RulesetBMS::updateRelease(released, t);

    RulesetBMS::update(t);
}

void RulesetBMSReplay::fail()
{
    _startTime = lunaticvibes::Time(0);
    _hasStartTime = true;
    RulesetBMS::fail();
}
