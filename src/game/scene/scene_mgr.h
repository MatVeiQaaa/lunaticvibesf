#pragma once

#include "game/scene/scene.h"

#include <memory>

namespace lunaticvibes
{

std::shared_ptr<SceneBase> buildScene(SceneType);

} // namespace lunaticvibes
