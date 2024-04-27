#include "replay_chart.h"
#include "common/types.h"
#include "common/utils.h"
#include "config/config_mgr.h"

#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>

#include <cstdlib>
#include <sstream>
#include <string_view>

// TODO: LVF version.
// TODO: play timestamp.
// TODO: player UUID?
// TODO: player name.

bool ReplayChart::loadFile(const Path& path)
{
	std::ifstream ifs(path, std::ios::binary);
	if (ifs.good())
	{
		try
		{
			cereal::PortableBinaryInputArchive ia(ifs);
			ia(*this);
		}
		catch (const cereal::Exception& e)
		{
			LOG_ERROR << "[ReplayChart] loadFile() cereal exception: " << e.what();
			return false;
		}
		const bool valid = validate();
		if (!valid)
		{
			LOG_DEBUG << "[ReplayChart] Loaded replay file is invalid";
		}
		return valid;
	}
	return false;
}

bool ReplayChart::loadXml(const std::string_view xml)
{
    std::stringstream ss;
    ss << xml;
    try
    {
        cereal::XMLInputArchive ia{ss};
        ia(*this);
    }
    catch (const cereal::Exception &e)
    {
        LOG_ERROR << "[ReplayChart] loadXml() cereal exception: " << e.what();
        return false;
    }
    const bool valid = validate();
    if (!valid)
    {
        LOG_DEBUG << "[ReplayChart] Loaded replay XML is invalid";
    }
    return valid;
}

bool ReplayChart::saveFile(const Path& path)
{
	fs::create_directories(path.parent_path());
	std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
	if (ofs.good())
	{
		updateChecksum();
		try
		{
			cereal::PortableBinaryOutputArchive oa(ofs);
			oa(*this);
		}
		catch (const cereal::Exception& e)
		{
			LOG_ERROR << "[ReplayChart] saveFile() cereal exception: " << e.what();
			return false;
		}
		return true;
	}
	return false;
}

std::string ReplayChart::serializeAsXml()
{
    std::stringstream ss;
    {
        try
        {
            cereal::XMLOutputArchive ia{ss};
            ia(*this);
        }
        catch (const cereal::Exception& e)
        {
            LOG_ERROR << "[ReplayChart] cereal XMLOutputArchive failed: " << e.what();
            return {};
        }
    }
    return ss.str();
}

bool ReplayChart::validate()
{
	uint32_t checksumReal = checksum;
	updateChecksum();
	bool valid = (checksum == checksumReal);
	checksum = checksumReal;
	return valid;
}

// NOTE: mutable this to set checksum to 0 before serializing
void ReplayChart::updateChecksum()
{
	checksum = 0;

	std::stringstream ss;
	try
	{
		cereal::PortableBinaryOutputArchive oa(ss);
		oa(*this);
	}
	catch (const cereal::Exception& e)
	{
		LOG_ERROR << "[ReplayChart] updateChecksum() cereal exception: " << e.what();
		return;
	}

	auto hash = md5(ss.str()).hexdigest().substr(0, 4);
	checksum |= base16(hash[0]) << 0;
	checksum |= base16(hash[1]) << 8;
	checksum |= base16(hash[2]) << 16;
	checksum |= base16(hash[3]) << 24;
}


Path ReplayChart::getReplayPath(const HashMD5& chartMD5)
{
	return ConfigMgr::Profile()->getPath() / "replay" / "chart" / chartMD5.hexdigest();
}

Path ReplayChart::getReplayPath()
{
	return getReplayPath(chartHash);
}

PlayModifiers ReplayChart::getMods() const
{
    PlayModifiers out;
    out.randomLeft = randomTypeLeft;
    out.randomRight = randomTypeRight;
    out.gauge = gaugeType;
    out.assist_mask = assistMask;
    out.hispeedFix = hispeedFix;
    out.laneEffect = (PlayModifierLaneEffectType)laneEffectType;
    out.DPFlip = DPFlip;
    return out;
}

const std::map<Input::Pad, ReplayChart::Commands::Type> REPLAY_INPUT_DOWN_CMD_MAP =
{
    { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_DOWN },
    { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_DOWN },
    { Input::Pad::K11,      ReplayChart::Commands::Type::K11_DOWN },
    { Input::Pad::K12,      ReplayChart::Commands::Type::K12_DOWN },
    { Input::Pad::K13,      ReplayChart::Commands::Type::K13_DOWN },
    { Input::Pad::K14,      ReplayChart::Commands::Type::K14_DOWN },
    { Input::Pad::K15,      ReplayChart::Commands::Type::K15_DOWN },
    { Input::Pad::K16,      ReplayChart::Commands::Type::K16_DOWN },
    { Input::Pad::K17,      ReplayChart::Commands::Type::K17_DOWN },
    { Input::Pad::K18,      ReplayChart::Commands::Type::K18_DOWN },
    { Input::Pad::K19,      ReplayChart::Commands::Type::K19_DOWN },
    { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_DOWN },
    { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_DOWN },
    { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_DOWN },
    { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_DOWN },
    { Input::Pad::K21,      ReplayChart::Commands::Type::K21_DOWN },
    { Input::Pad::K22,      ReplayChart::Commands::Type::K22_DOWN },
    { Input::Pad::K23,      ReplayChart::Commands::Type::K23_DOWN },
    { Input::Pad::K24,      ReplayChart::Commands::Type::K24_DOWN },
    { Input::Pad::K25,      ReplayChart::Commands::Type::K25_DOWN },
    { Input::Pad::K26,      ReplayChart::Commands::Type::K26_DOWN },
    { Input::Pad::K27,      ReplayChart::Commands::Type::K27_DOWN },
    { Input::Pad::K28,      ReplayChart::Commands::Type::K28_DOWN },
    { Input::Pad::K29,      ReplayChart::Commands::Type::K29_DOWN },
    { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_DOWN },
    { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_DOWN },
};

const std::map<Input::Pad, ReplayChart::Commands::Type> REPLAY_INPUT_UP_CMD_MAP =
{
    { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_UP },
    { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_UP },
    { Input::Pad::K11,      ReplayChart::Commands::Type::K11_UP },
    { Input::Pad::K12,      ReplayChart::Commands::Type::K12_UP },
    { Input::Pad::K13,      ReplayChart::Commands::Type::K13_UP },
    { Input::Pad::K14,      ReplayChart::Commands::Type::K14_UP },
    { Input::Pad::K15,      ReplayChart::Commands::Type::K15_UP },
    { Input::Pad::K16,      ReplayChart::Commands::Type::K16_UP },
    { Input::Pad::K17,      ReplayChart::Commands::Type::K17_UP },
    { Input::Pad::K18,      ReplayChart::Commands::Type::K18_UP },
    { Input::Pad::K19,      ReplayChart::Commands::Type::K19_UP },
    { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_UP },
    { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_UP },
    { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_UP },
    { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_UP },
    { Input::Pad::K21,      ReplayChart::Commands::Type::K21_UP },
    { Input::Pad::K22,      ReplayChart::Commands::Type::K22_UP },
    { Input::Pad::K23,      ReplayChart::Commands::Type::K23_UP },
    { Input::Pad::K24,      ReplayChart::Commands::Type::K24_UP },
    { Input::Pad::K25,      ReplayChart::Commands::Type::K25_UP },
    { Input::Pad::K26,      ReplayChart::Commands::Type::K26_UP },
    { Input::Pad::K27,      ReplayChart::Commands::Type::K27_UP },
    { Input::Pad::K28,      ReplayChart::Commands::Type::K28_UP },
    { Input::Pad::K29,      ReplayChart::Commands::Type::K29_UP },
    { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_UP },
    { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_UP },
};

const std::map<Input::Pad, ReplayChart::Commands::Type> REPLAY_INPUT_DOWN_CMD_MAP_5K[4] =
{
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_DOWN },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_DOWN },
        { Input::Pad::K11,      ReplayChart::Commands::Type::K11_DOWN },
        { Input::Pad::K12,      ReplayChart::Commands::Type::K12_DOWN },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K13_DOWN },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K14_DOWN },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K15_DOWN },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_DOWN },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_DOWN },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_DOWN },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_DOWN },
        { Input::Pad::K21,      ReplayChart::Commands::Type::K21_DOWN },
        { Input::Pad::K22,      ReplayChart::Commands::Type::K22_DOWN },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K23_DOWN },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K24_DOWN },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K25_DOWN },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_DOWN },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_DOWN },
    },
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_DOWN },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_DOWN },
        { Input::Pad::K11,      ReplayChart::Commands::Type::K11_DOWN },
        { Input::Pad::K12,      ReplayChart::Commands::Type::K12_DOWN },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K13_DOWN },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K14_DOWN },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K15_DOWN },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_DOWN },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_DOWN },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_DOWN },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_DOWN },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K21_DOWN },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K22_DOWN },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K23_DOWN },
        { Input::Pad::K26,      ReplayChart::Commands::Type::K24_DOWN },
        { Input::Pad::K27,      ReplayChart::Commands::Type::K25_DOWN },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_DOWN },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_DOWN },
    },
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_DOWN },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_DOWN },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K11_DOWN },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K12_DOWN },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K13_DOWN },
        { Input::Pad::K16,      ReplayChart::Commands::Type::K14_DOWN },
        { Input::Pad::K17,      ReplayChart::Commands::Type::K15_DOWN },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_DOWN },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_DOWN },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_DOWN },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_DOWN },
        { Input::Pad::K21,      ReplayChart::Commands::Type::K21_DOWN },
        { Input::Pad::K22,      ReplayChart::Commands::Type::K22_DOWN },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K23_DOWN },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K24_DOWN },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K25_DOWN },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_DOWN },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_DOWN },
    },
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_DOWN },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_DOWN },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K11_DOWN },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K12_DOWN },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K13_DOWN },
        { Input::Pad::K16,      ReplayChart::Commands::Type::K14_DOWN },
        { Input::Pad::K17,      ReplayChart::Commands::Type::K15_DOWN },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_DOWN },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_DOWN },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_DOWN },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_DOWN },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K21_DOWN },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K22_DOWN },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K23_DOWN },
        { Input::Pad::K26,      ReplayChart::Commands::Type::K24_DOWN },
        { Input::Pad::K27,      ReplayChart::Commands::Type::K25_DOWN },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_DOWN },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_DOWN },
    },
};

const std::map<Input::Pad, ReplayChart::Commands::Type> REPLAY_INPUT_UP_CMD_MAP_5K[4] =
{
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_UP },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_UP },
        { Input::Pad::K11,      ReplayChart::Commands::Type::K11_UP },
        { Input::Pad::K12,      ReplayChart::Commands::Type::K12_UP },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K13_UP },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K14_UP },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K15_UP },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_UP },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_UP },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_UP },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_UP },
        { Input::Pad::K21,      ReplayChart::Commands::Type::K21_UP },
        { Input::Pad::K22,      ReplayChart::Commands::Type::K22_UP },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K23_UP },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K24_UP },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K25_UP },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_UP },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_UP },
    },
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_UP },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_UP },
        { Input::Pad::K11,      ReplayChart::Commands::Type::K11_UP },
        { Input::Pad::K12,      ReplayChart::Commands::Type::K12_UP },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K13_UP },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K14_UP },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K15_UP },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_UP },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_UP },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_UP },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_UP },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K21_UP },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K22_UP },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K23_UP },
        { Input::Pad::K26,      ReplayChart::Commands::Type::K24_UP },
        { Input::Pad::K27,      ReplayChart::Commands::Type::K25_UP },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_UP },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_UP },
    },
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_UP },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_UP },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K11_UP },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K12_UP },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K13_UP },
        { Input::Pad::K16,      ReplayChart::Commands::Type::K14_UP },
        { Input::Pad::K17,      ReplayChart::Commands::Type::K15_UP },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_UP },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_UP },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_UP },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_UP },
        { Input::Pad::K21,      ReplayChart::Commands::Type::K21_UP },
        { Input::Pad::K22,      ReplayChart::Commands::Type::K22_UP },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K23_UP },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K24_UP },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K25_UP },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_UP },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_UP },
    },
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_UP },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_UP },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K11_UP },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K12_UP },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K13_UP },
        { Input::Pad::K16,      ReplayChart::Commands::Type::K14_UP },
        { Input::Pad::K17,      ReplayChart::Commands::Type::K15_UP },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_UP },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_UP },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_UP },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_UP },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K21_UP },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K22_UP },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K23_UP },
        { Input::Pad::K26,      ReplayChart::Commands::Type::K24_UP },
        { Input::Pad::K27,      ReplayChart::Commands::Type::K25_UP },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_UP },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_UP },
    },
};

const std::map<ReplayChart::Commands::Type, Input::Pad> REPLAY_CMD_INPUT_DOWN_MAP =
{
    { ReplayChart::Commands::Type::S1L_DOWN, Input::Pad::S1L },
    { ReplayChart::Commands::Type::S1R_DOWN, Input::Pad::S1R },
    { ReplayChart::Commands::Type::K11_DOWN, Input::Pad::K11 },
    { ReplayChart::Commands::Type::K12_DOWN, Input::Pad::K12 },
    { ReplayChart::Commands::Type::K13_DOWN, Input::Pad::K13 },
    { ReplayChart::Commands::Type::K14_DOWN, Input::Pad::K14 },
    { ReplayChart::Commands::Type::K15_DOWN, Input::Pad::K15 },
    { ReplayChart::Commands::Type::K16_DOWN, Input::Pad::K16 },
    { ReplayChart::Commands::Type::K17_DOWN, Input::Pad::K17 },
    { ReplayChart::Commands::Type::K18_DOWN, Input::Pad::K18 },
    { ReplayChart::Commands::Type::K19_DOWN, Input::Pad::K19 },
    { ReplayChart::Commands::Type::K1START_DOWN, Input::Pad::K1START },
    { ReplayChart::Commands::Type::K1SELECT_DOWN, Input::Pad::K1SELECT },
    { ReplayChart::Commands::Type::S2L_DOWN, Input::Pad::S2L },
    { ReplayChart::Commands::Type::S2R_DOWN, Input::Pad::S2R },
    { ReplayChart::Commands::Type::K21_DOWN, Input::Pad::K21 },
    { ReplayChart::Commands::Type::K22_DOWN, Input::Pad::K22 },
    { ReplayChart::Commands::Type::K23_DOWN, Input::Pad::K23 },
    { ReplayChart::Commands::Type::K24_DOWN, Input::Pad::K24 },
    { ReplayChart::Commands::Type::K25_DOWN, Input::Pad::K25 },
    { ReplayChart::Commands::Type::K26_DOWN, Input::Pad::K26 },
    { ReplayChart::Commands::Type::K27_DOWN, Input::Pad::K27 },
    { ReplayChart::Commands::Type::K28_DOWN, Input::Pad::K28 },
    { ReplayChart::Commands::Type::K29_DOWN, Input::Pad::K29 },
    { ReplayChart::Commands::Type::K2START_DOWN, Input::Pad::K2START },
    { ReplayChart::Commands::Type::K2SELECT_DOWN, Input::Pad::K2SELECT },
};

const std::map<ReplayChart::Commands::Type, Input::Pad> REPLAY_CMD_INPUT_UP_MAP =
{
    { ReplayChart::Commands::Type::S1L_UP, Input::Pad::S1L },
    { ReplayChart::Commands::Type::S1R_UP, Input::Pad::S1R },
    { ReplayChart::Commands::Type::K11_UP, Input::Pad::K11 },
    { ReplayChart::Commands::Type::K12_UP, Input::Pad::K12 },
    { ReplayChart::Commands::Type::K13_UP, Input::Pad::K13 },
    { ReplayChart::Commands::Type::K14_UP, Input::Pad::K14 },
    { ReplayChart::Commands::Type::K15_UP, Input::Pad::K15 },
    { ReplayChart::Commands::Type::K16_UP, Input::Pad::K16 },
    { ReplayChart::Commands::Type::K17_UP, Input::Pad::K17 },
    { ReplayChart::Commands::Type::K18_UP, Input::Pad::K18 },
    { ReplayChart::Commands::Type::K19_UP, Input::Pad::K19 },
    { ReplayChart::Commands::Type::K1START_UP, Input::Pad::K1START },
    { ReplayChart::Commands::Type::K1SELECT_UP, Input::Pad::K1SELECT },
    { ReplayChart::Commands::Type::S2L_UP, Input::Pad::S2L },
    { ReplayChart::Commands::Type::S2R_UP, Input::Pad::S2R },
    { ReplayChart::Commands::Type::K21_UP, Input::Pad::K21 },
    { ReplayChart::Commands::Type::K22_UP, Input::Pad::K22 },
    { ReplayChart::Commands::Type::K23_UP, Input::Pad::K23 },
    { ReplayChart::Commands::Type::K24_UP, Input::Pad::K24 },
    { ReplayChart::Commands::Type::K25_UP, Input::Pad::K25 },
    { ReplayChart::Commands::Type::K26_UP, Input::Pad::K26 },
    { ReplayChart::Commands::Type::K27_UP, Input::Pad::K27 },
    { ReplayChart::Commands::Type::K28_UP, Input::Pad::K28 },
    { ReplayChart::Commands::Type::K29_UP, Input::Pad::K29 },
    { ReplayChart::Commands::Type::K2START_UP, Input::Pad::K2START },
    { ReplayChart::Commands::Type::K2SELECT_UP, Input::Pad::K2SELECT },
};

const std::map<ReplayChart::Commands::Type, Input::Pad> REPLAY_CMD_INPUT_DOWN_MAP_5K[4] =
{
    {
        { ReplayChart::Commands::Type::S1L_DOWN, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_DOWN, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_DOWN, Input::Pad::K11 },
        { ReplayChart::Commands::Type::K12_DOWN, Input::Pad::K12 },
        { ReplayChart::Commands::Type::K13_DOWN, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K14_DOWN, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K15_DOWN, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K1START_DOWN, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_DOWN, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_DOWN, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_DOWN, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_DOWN, Input::Pad::K21 },
        { ReplayChart::Commands::Type::K22_DOWN, Input::Pad::K22 },
        { ReplayChart::Commands::Type::K23_DOWN, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K24_DOWN, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K25_DOWN, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K2START_DOWN, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_DOWN, Input::Pad::K2SELECT },
    },
    {
        { ReplayChart::Commands::Type::S1L_DOWN, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_DOWN, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_DOWN, Input::Pad::K11 },
        { ReplayChart::Commands::Type::K12_DOWN, Input::Pad::K12 },
        { ReplayChart::Commands::Type::K13_DOWN, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K14_DOWN, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K15_DOWN, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K1START_DOWN, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_DOWN, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_DOWN, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_DOWN, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_DOWN, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K22_DOWN, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K23_DOWN, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K24_DOWN, Input::Pad::K26 },
        { ReplayChart::Commands::Type::K25_DOWN, Input::Pad::K27 },
        { ReplayChart::Commands::Type::K2START_DOWN, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_DOWN, Input::Pad::K2SELECT },
    },
    {
        { ReplayChart::Commands::Type::S1L_DOWN, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_DOWN, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_DOWN, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K12_DOWN, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K13_DOWN, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K14_DOWN, Input::Pad::K16 },
        { ReplayChart::Commands::Type::K15_DOWN, Input::Pad::K17 },
        { ReplayChart::Commands::Type::K1START_DOWN, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_DOWN, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_DOWN, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_DOWN, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_DOWN, Input::Pad::K21 },
        { ReplayChart::Commands::Type::K22_DOWN, Input::Pad::K22 },
        { ReplayChart::Commands::Type::K23_DOWN, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K24_DOWN, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K25_DOWN, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K2START_DOWN, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_DOWN, Input::Pad::K2SELECT },
    },
    {
        { ReplayChart::Commands::Type::S1L_DOWN, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_DOWN, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_DOWN, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K12_DOWN, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K13_DOWN, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K14_DOWN, Input::Pad::K16 },
        { ReplayChart::Commands::Type::K15_DOWN, Input::Pad::K17 },
        { ReplayChart::Commands::Type::K1START_DOWN, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_DOWN, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_DOWN, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_DOWN, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_DOWN, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K22_DOWN, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K23_DOWN, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K24_DOWN, Input::Pad::K26 },
        { ReplayChart::Commands::Type::K25_DOWN, Input::Pad::K27 },
        { ReplayChart::Commands::Type::K2START_DOWN, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_DOWN, Input::Pad::K2SELECT },
    },
};

const std::map<ReplayChart::Commands::Type, Input::Pad> REPLAY_CMD_INPUT_UP_MAP_5K[4] =
{
    {
        { ReplayChart::Commands::Type::S1L_UP, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_UP, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_UP, Input::Pad::K11 },
        { ReplayChart::Commands::Type::K12_UP, Input::Pad::K12 },
        { ReplayChart::Commands::Type::K13_UP, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K14_UP, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K15_UP, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K1START_UP, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_UP, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_UP, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_UP, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_UP, Input::Pad::K21 },
        { ReplayChart::Commands::Type::K22_UP, Input::Pad::K22 },
        { ReplayChart::Commands::Type::K23_UP, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K24_UP, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K25_UP, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K2START_UP, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_UP, Input::Pad::K2SELECT },
    },
    {
        { ReplayChart::Commands::Type::S1L_UP, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_UP, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_UP, Input::Pad::K11 },
        { ReplayChart::Commands::Type::K12_UP, Input::Pad::K12 },
        { ReplayChart::Commands::Type::K13_UP, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K14_UP, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K15_UP, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K1START_UP, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_UP, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_UP, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_UP, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_UP, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K22_UP, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K23_UP, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K24_UP, Input::Pad::K26 },
        { ReplayChart::Commands::Type::K25_UP, Input::Pad::K27 },
        { ReplayChart::Commands::Type::K2START_UP, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_UP, Input::Pad::K2SELECT },
    },
    {
        { ReplayChart::Commands::Type::S1L_UP, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_UP, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_UP, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K12_UP, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K13_UP, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K14_UP, Input::Pad::K16 },
        { ReplayChart::Commands::Type::K15_UP, Input::Pad::K17 },
        { ReplayChart::Commands::Type::K1START_UP, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_UP, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_UP, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_UP, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_UP, Input::Pad::K21 },
        { ReplayChart::Commands::Type::K22_UP, Input::Pad::K22 },
        { ReplayChart::Commands::Type::K23_UP, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K24_UP, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K25_UP, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K2START_UP, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_UP, Input::Pad::K2SELECT },
    },
    {
        { ReplayChart::Commands::Type::S1L_UP, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_UP, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_UP, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K12_UP, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K13_UP, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K14_UP, Input::Pad::K16 },
        { ReplayChart::Commands::Type::K15_UP, Input::Pad::K17 },
        { ReplayChart::Commands::Type::K1START_UP, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_UP, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_UP, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_UP, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_UP, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K22_UP, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K23_UP, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K24_UP, Input::Pad::K26 },
        { ReplayChart::Commands::Type::K25_UP, Input::Pad::K27 },
        { ReplayChart::Commands::Type::K2START_UP, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_UP, Input::Pad::K2SELECT },
    },
};

ReplayChart::Commands::Type ReplayChart::Commands::leftSideCmdToRightSide(const ReplayChart::Commands::Type cmd)
{
    using CmdType = ReplayChart::Commands::Type;
    switch (cmd)
    {
    case CmdType::S1L_DOWN: return CmdType::S2L_DOWN;
    case CmdType::S1R_DOWN: return CmdType::S2R_DOWN;
    case CmdType::K11_DOWN: return CmdType::K21_DOWN;
    case CmdType::K12_DOWN: return CmdType::K22_DOWN;
    case CmdType::K13_DOWN: return CmdType::K23_DOWN;
    case CmdType::K14_DOWN: return CmdType::K24_DOWN;
    case CmdType::K15_DOWN: return CmdType::K25_DOWN;
    case CmdType::K16_DOWN: return CmdType::K26_DOWN;
    case CmdType::K17_DOWN: return CmdType::K27_DOWN;
    case CmdType::K18_DOWN: return CmdType::K28_DOWN;
    case CmdType::K19_DOWN: return CmdType::K29_DOWN;
    case CmdType::S1L_UP: return CmdType::S2L_UP;
    case CmdType::S1R_UP: return CmdType::S2R_UP;
    case CmdType::K11_UP: return CmdType::K21_UP;
    case CmdType::K12_UP: return CmdType::K22_UP;
    case CmdType::K13_UP: return CmdType::K23_UP;
    case CmdType::K14_UP: return CmdType::K24_UP;
    case CmdType::K15_UP: return CmdType::K25_UP;
    case CmdType::K16_UP: return CmdType::K26_UP;
    case CmdType::K17_UP: return CmdType::K27_UP;
    case CmdType::K18_UP: return CmdType::K28_UP;
    case CmdType::K19_UP: return CmdType::K29_UP;
    case CmdType::S1A_PLUS: return CmdType::S2A_PLUS;
    case CmdType::S1A_MINUS: return CmdType::S2A_MINUS;
    case CmdType::S1A_STOP: return CmdType::S2A_STOP;
    case CmdType::JUDGE_LEFT_EXACT_0: return CmdType::JUDGE_RIGHT_EXACT_0;
    case CmdType::JUDGE_LEFT_EARLY_0: return CmdType::JUDGE_RIGHT_EARLY_0;
    case CmdType::JUDGE_LEFT_EARLY_1: return CmdType::JUDGE_RIGHT_EARLY_1;
    case CmdType::JUDGE_LEFT_EARLY_2: return CmdType::JUDGE_RIGHT_EARLY_2;
    case CmdType::JUDGE_LEFT_EARLY_3: return CmdType::JUDGE_RIGHT_EARLY_3;
    case CmdType::JUDGE_LEFT_EARLY_4: return CmdType::JUDGE_RIGHT_EARLY_4;
    case CmdType::JUDGE_LEFT_EARLY_5: return CmdType::JUDGE_RIGHT_EARLY_5;
    case CmdType::JUDGE_LEFT_LATE_0: return CmdType::JUDGE_RIGHT_LATE_0;
    case CmdType::JUDGE_LEFT_LATE_1: return CmdType::JUDGE_RIGHT_LATE_1;
    case CmdType::JUDGE_LEFT_LATE_2: return CmdType::JUDGE_RIGHT_LATE_2;
    case CmdType::JUDGE_LEFT_LATE_3: return CmdType::JUDGE_RIGHT_LATE_3;
    case CmdType::JUDGE_LEFT_LATE_4: return CmdType::JUDGE_RIGHT_LATE_4;
    case CmdType::JUDGE_LEFT_LATE_5: return CmdType::JUDGE_RIGHT_LATE_5;
    case CmdType::JUDGE_LEFT_LANDMINE: return CmdType::JUDGE_RIGHT_LANDMINE;
    default: return cmd;
    }
    std::abort(); // unreachable
}
