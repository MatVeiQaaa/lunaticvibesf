#pragma once
#include "skin.h"
#include <array>
#include <memory>

class SkinMgr
{
public:
    SkinMgr();
    SkinMgr& operator=(SkinMgr&&) = delete;
    SkinMgr& operator=(SkinMgr&) = delete;
    SkinMgr(SkinMgr&&) = delete;
    SkinMgr(SkinMgr&) = delete;

public:
    void reload(SkinType, bool simple = false);
    void unload(SkinType);
    /// May return `nullptr` if that skin has not been loaded yet.
    std::shared_ptr<SkinBase> get(SkinType);

protected:
    std::array<std::shared_ptr<SkinBase>, static_cast<size_t>(SkinType::MODE_COUNT)> _skins{};
    std::shared_ptr<std::array<std::shared_ptr<SpriteBase>, SPRITE_GLOBAL_MAX>> _sharedSprites{};
};