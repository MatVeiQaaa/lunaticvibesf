#include "skin_mgr.h"
#include "common/utils.h"
#include "config/config_mgr.h"
#include "game/skin/skin.h"
#include "game/skin/skin_lr2.h"
#include <memory>

SkinMgr::SkinMgr() : _sharedSprites(std::make_shared<std::array<std::shared_ptr<SpriteBase>, SPRITE_GLOBAL_MAX>>())
{
}

void SkinMgr::reload(SkinType e, bool simple)
{
    auto& skinObj = _skins[static_cast<size_t>(e)];
    if (skinObj != nullptr)
        unload(e);

    std::string skinFilePathStr;
    Path skinFilePath;
    Path skinFilePathDefault;
    SkinVersion version = SkinVersion::LR2beta3;

    switch (e)
    {
    case SkinType::MUSIC_SELECT:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_MUSIC_SELECT;
        skinFilePathStr = ConfigMgr::get('S', cfg::S_PATH_MUSIC_SELECT, cfg::S_DEFAULT_PATH_MUSIC_SELECT);
        break;
    case SkinType::DECIDE:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_DECIDE;
        skinFilePathStr = ConfigMgr::get('S', cfg::S_PATH_DECIDE, cfg::S_DEFAULT_PATH_DECIDE);
        break;
    case SkinType::RESULT:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_RESULT;
        skinFilePathStr = ConfigMgr::get('S', cfg::S_PATH_RESULT, cfg::S_DEFAULT_PATH_RESULT);
        break;
    case SkinType::COURSE_RESULT:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_COURSE_RESULT;
        skinFilePathStr = ConfigMgr::get('S', cfg::S_PATH_COURSE_RESULT, cfg::S_DEFAULT_PATH_COURSE_RESULT);
        break;
    case SkinType::KEY_CONFIG:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_KEYCONFIG;
        skinFilePathStr = ConfigMgr::get('S', cfg::S_PATH_KEYCONFIG, cfg::S_DEFAULT_PATH_KEYCONFIG);
        break;
    case SkinType::THEME_SELECT:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_CUSTOMIZE;
        skinFilePathStr = ConfigMgr::get('S', cfg::S_PATH_CUSTOMIZE, cfg::S_DEFAULT_PATH_CUSTOMIZE);
        break;
    case SkinType::PLAY5:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_5;
        skinFilePathStr = ConfigMgr::get('S', cfg::S_PATH_PLAY_5, cfg::S_DEFAULT_PATH_PLAY_5);
        break;
    case SkinType::PLAY5_2:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_5_BATTLE;
        skinFilePathStr = ConfigMgr::get('S', cfg::S_PATH_PLAY_5_BATTLE, cfg::S_DEFAULT_PATH_PLAY_5_BATTLE);
        break;
    case SkinType::PLAY7:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_7;
        skinFilePathStr = ConfigMgr::get('S', cfg::S_PATH_PLAY_7, cfg::S_DEFAULT_PATH_PLAY_7);
        break;
    case SkinType::PLAY7_2:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_7_BATTLE;
        skinFilePathStr = ConfigMgr::get('S', cfg::S_PATH_PLAY_7_BATTLE, cfg::S_DEFAULT_PATH_PLAY_7_BATTLE);
        break;
    case SkinType::PLAY9:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_9;
        skinFilePathStr = ConfigMgr::get('S', cfg::S_PATH_PLAY_9, cfg::S_DEFAULT_PATH_PLAY_9);
        break;
    case SkinType::PLAY10:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_10;
        skinFilePathStr = ConfigMgr::get('S', cfg::S_PATH_PLAY_10, cfg::S_DEFAULT_PATH_PLAY_10);
        break;
    case SkinType::PLAY14:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_14;
        skinFilePathStr = ConfigMgr::get('S', cfg::S_PATH_PLAY_14, cfg::S_DEFAULT_PATH_PLAY_14);
        break;
    case SkinType::EXIT:
    case SkinType::TITLE:
    case SkinType::SOUNDSET:
    case SkinType::PLAY9_2:
    case SkinType::RETRY_TRANS:
    case SkinType::COURSE_TRANS:
    case SkinType::EXIT_TRANS:
    case SkinType::PRE_SELECT:
    case SkinType::TMPL:
    case SkinType::TEST:
    case SkinType::MODE_COUNT: version = SkinVersion::UNDEF; break;
    }

    skinFilePath = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), skinFilePathStr));

    switch (version)
    {
    case SkinVersion::LR2beta3:
        skinObj = std::make_shared<SkinLR2>(_sharedSprites, skinFilePath, simple ? 1 : 0);
        if (!skinObj->isLoaded())
            skinObj = std::make_shared<SkinLR2>(_sharedSprites, skinFilePathDefault, simple ? 1 : 0);
        break;
    case SkinVersion::UNDEF: break;
    }
}

std::shared_ptr<SkinBase> SkinMgr::get(SkinType e)
{
    return _skins[static_cast<size_t>(e)];
}
void SkinMgr::unload(SkinType e)
{
    _skins[static_cast<size_t>(e)].reset();
}