#include "arena_data.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "game/arena/arena_client.h"
#include "game/arena/arena_host.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms_network.h"
#include "game/scene/scene_context.h"

ArenaData gArenaData;

void ArenaData::reset()
{
    online = false;
    expired = false;
    playing = false;
    data.clear();
    playerIDs.clear();
    playStartTimeMs = 0;
    playingFinished = false;
    ready = false;

    if (g_pArenaClient)
    {
        g_pArenaClient->loopEnd();
        g_pArenaClient.reset();
    }
    if (g_pArenaHost)
    {
        g_pArenaHost->loopEnd();
        g_pArenaHost.reset();
    }
}

size_t ArenaData::getPlayerCount()
{
    return playerIDs.size();
}

const std::string& ArenaData::getPlayerName(size_t index)
{
    static const std::string empty = "";
    return index < getPlayerCount() ? data[playerIDs[index]].name : empty;
}

std::shared_ptr<vRulesetNetwork> ArenaData::getPlayerRuleset(size_t index)
{
    return index < getPlayerCount() && playing ? data[playerIDs[index]].ruleset : nullptr;
}

int ArenaData::getPlayerID(size_t index)
{
    return index < getPlayerCount() ? playerIDs[index] : -1;
}

bool ArenaData::isSelfReady()
{
    return ready;
}

bool ArenaData::isPlayerReady(size_t index)
{
    return index < getPlayerCount() ? data[playerIDs[index]].ready : false;
}

void ArenaData::initPlaying(RulesetType rulesetType)
{
    const unsigned keys = lunaticvibes::skinTypeToKeys(gPlayContext.mode);
    unsigned idx = 0;
    for (auto& [id, d] : data)
    {
        switch (rulesetType)
        {
        case RulesetType::SOUND_ONLY: break;
        case RulesetType::BMS: d.ruleset = std::make_shared<RulesetBMSNetwork>(keys, idx++); break;
        }
    }
}

void ArenaData::startPlaying()
{
    playing = true;
}

void ArenaData::stopPlaying()
{
    ready = false;
    playStartTimeMs = 0;
    playingFinished = false;
    playing = false;

    for (auto& [id, d] : data)
    {
        d.ready = false;
        d.ruleset.reset();
    }
}

void ArenaData::updateTexts()
{
    State::set(IndexText::ARENA_PLAYER_NAME_1, gArenaData.getPlayerName(0));
    State::set(IndexText::ARENA_PLAYER_NAME_2, gArenaData.getPlayerName(1));
    State::set(IndexText::ARENA_PLAYER_NAME_3, gArenaData.getPlayerName(2));
    State::set(IndexText::ARENA_PLAYER_NAME_4, gArenaData.getPlayerName(3));
    State::set(IndexText::ARENA_PLAYER_NAME_5, gArenaData.getPlayerName(4));
    State::set(IndexText::ARENA_PLAYER_NAME_6, gArenaData.getPlayerName(5));
    State::set(IndexText::ARENA_PLAYER_NAME_7, gArenaData.getPlayerName(6));
    State::set(IndexText::ARENA_PLAYER_NAME_8, gArenaData.getPlayerName(7));

    for (size_t i = 0; i < MAX_ARENA_PLAYERS; ++i)
    {
        if (auto prb = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(i)); prb)
        {
            State::set(IndexText(size_t(IndexText::ARENA_MODIFIER_1) + i), prb->getModifierText());
            State::set(IndexText(size_t(IndexText::ARENA_MODIFIER_SHORT_1) + i), prb->getModifierTextShort());
        }
    }
}

void ArenaData::updateGlobals()
{
    lunaticvibes::Time t;
    std::vector<std::pair<unsigned, IndexOption>> ranking;
    for (size_t i = 0; i < getPlayerCount(); ++i)
    {
        auto& d = data[playerIDs[i]];
        if (d.ruleset)
        {
            d.ruleset->update(t);
            if (auto p = std::dynamic_pointer_cast<RulesetBMS>(d.ruleset); p)
            {
                size_t offset = (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * i;
                ranking.emplace_back(p->getExScore(), IndexOption(int(IndexOption::ARENA_PLAYDATA_RANKING) + offset));
            }
        }
    }
    ranking.emplace_back(State::get(IndexNumber::PLAY_1P_EXSCORE), IndexOption::RESULT_ARENA_PLAYER_RANKING);

    std::sort(ranking.begin(), ranking.end());
    int rank = 1;
    int step = 0;
    unsigned exscorePrev = 0;
    for (auto it = ranking.rbegin(); it != ranking.rend(); ++it)
    {
        auto& [exscore, op] = *it;
        if (exscore == exscorePrev)
        {
            step++;
        }
        else
        {
            rank += step;
            exscorePrev = exscore;
            step = 1;
        }
        State::set(op, rank);
    }
}