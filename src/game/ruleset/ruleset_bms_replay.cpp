#include "ruleset_bms_replay.h"

#include <cassert>
#include <utility>

RulesetBMSReplay::RulesetBMSReplay(std::shared_ptr<ChartFormatBase> format_, std::shared_ptr<ChartObjectBase> chart_,
                                   std::shared_ptr<ReplayChart> replay_, const PlayModifiers mods, GameModeKeys keys,
                                   JudgeDifficulty difficulty, double health, PlaySide side, const int fiveKeyMapIndex,
                                   const double pitchSpeed)
    : RulesetBase(format_, chart_),
      RulesetBMS(std::move(format_), std::move(chart_), mods, keys, difficulty, health, side, fiveKeyMapIndex),
      replay(std::move(replay_))
{
    assert(side == PlaySide::AUTO || side == PlaySide::AUTO_DOUBLE || side == PlaySide::AUTO_2P || side == PlaySide::RIVAL || side == PlaySide::MYBEST);

    if (fiveKeyMapIndex == -1)
    {
        _inputDownMap = &REPLAY_CMD_INPUT_DOWN_MAP;
        _inputUpMap = &REPLAY_CMD_INPUT_UP_MAP;
    }
    else
    {
        _inputDownMap = &REPLAY_CMD_INPUT_DOWN_MAP_5K[fiveKeyMapIndex];
        _inputUpMap = &REPLAY_CMD_INPUT_UP_MAP_5K[fiveKeyMapIndex];
    }

    itReplayCommand = replay->commands.begin();
    showJudge = (_side == PlaySide::AUTO || _side == PlaySide::AUTO_DOUBLE || _side == PlaySide::AUTO_2P);

    if (replay->pitchValue != 0)
    {
        static const double tick = std::pow(2, 1.0 / 12);
        replayTimestampMultiplier = std::pow(tick, replay->pitchValue) / pitchSpeed;
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

    const InputMask prevKeyPressing = keyPressing;
    while (itReplayCommand != replay->commands.end() && rt.norm() >= (long long)std::round(itReplayCommand->ms * replayTimestampMultiplier))
    {
        auto cmd = itReplayCommand->type;
        if (_side == PlaySide::AUTO_2P)
        {
            cmd = ReplayChart::Commands::leftSideCmdToRightSide(cmd);
        }

        if (!skipToEnd)
        {
            if (auto it = _inputDownMap->find(cmd); it != _inputDownMap->end())
            {
                keyPressing[it->second] = true;
            }
            else if (auto it = _inputUpMap->find(cmd); it != _inputUpMap->end())
            {
                keyPressing[it->second] = false;
            }

            switch (cmd)
            {
            case ReplayChart::Commands::Type::S1A_PLUS:  playerScratchAccumulator[PLAYER_SLOT_PLAYER] = 0.0015; break;
            case ReplayChart::Commands::Type::S1A_MINUS: playerScratchAccumulator[PLAYER_SLOT_PLAYER] = -0.0015; break;
            case ReplayChart::Commands::Type::S1A_STOP:  playerScratchAccumulator[PLAYER_SLOT_PLAYER] = 0; break;
            case ReplayChart::Commands::Type::S2A_PLUS:  playerScratchAccumulator[PLAYER_SLOT_TARGET] = 0.0015; break;
            case ReplayChart::Commands::Type::S2A_MINUS: playerScratchAccumulator[PLAYER_SLOT_TARGET] = -0.0015; break;
            case ReplayChart::Commands::Type::S2A_STOP:  playerScratchAccumulator[PLAYER_SLOT_TARGET] = 0; break;
            default: break;
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
