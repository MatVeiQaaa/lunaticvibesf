#pragma once

#include "game/scene/scene.h"
#include "game/skin/skin_mgr.h"

#include <memory>

namespace lunaticvibes
{

std::shared_ptr<SceneBase> buildScene(const std::shared_ptr<SkinMgr>& skinMgr, SceneType);

} // namespace lunaticvibes
