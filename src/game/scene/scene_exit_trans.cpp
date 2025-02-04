#include "scene_exit_trans.h"
#include "common/log.h"
#include "scene_context.h"

SceneExitTrans::SceneExitTrans() : SceneBase(nullptr, SkinType::EXIT_TRANS, 240)
{
    _type = SceneType::EXIT_TRANS;

    LOG_DEBUG << "[ExitTrans]";
    gNextScene = SceneType::EXIT;
}
