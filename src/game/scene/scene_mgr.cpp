#include "scene_mgr.h"

#include <memory>

#include "game/scene/scene.h"
#include "game/scene/scene_course_result.h"
#include "game/scene/scene_customize.h"
#include "game/scene/scene_decide.h"
#include "game/scene/scene_exit_trans.h"
#include "game/scene/scene_keyconfig.h"
#include "game/scene/scene_play.h"
#include "game/scene/scene_play_course_trans.h"
#include "game/scene/scene_play_retry_trans.h"
#include "game/scene/scene_pre_select.h"
#include "game/scene/scene_result.h"
#include "game/scene/scene_select.h"

std::shared_ptr<SceneBase> lunaticvibes::buildScene(SceneType type)
{
    switch (type)
    {
    case SceneType::EXIT:
    case SceneType::NOT_INIT: break;
    case SceneType::PRE_SELECT: return std::make_shared<ScenePreSelect>();
    case SceneType::SELECT: return std::make_shared<SceneSelect>();
    case SceneType::DECIDE: return std::make_shared<SceneDecide>();
    case SceneType::PLAY: return std::make_shared<ScenePlay>();
    case SceneType::RETRY_TRANS: return std::make_shared<ScenePlayRetryTrans>();
    case SceneType::RESULT: return std::make_shared<SceneResult>();
    case SceneType::COURSE_TRANS: return std::make_shared<ScenePlayCourseTrans>();
    case SceneType::KEYCONFIG: return std::make_shared<SceneKeyConfig>();
    case SceneType::CUSTOMIZE: return std::make_shared<SceneCustomize>();
    case SceneType::COURSE_RESULT: return std::make_shared<SceneCourseResult>();
    case SceneType::EXIT_TRANS: return std::make_shared<SceneExitTrans>();
    }
    assert(false);
    return nullptr;
}