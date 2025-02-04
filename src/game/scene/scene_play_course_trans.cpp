#include "scene_play_course_trans.h"

#include "common/log.h"
#include "scene_context.h"

ScenePlayCourseTrans::ScenePlayCourseTrans() : SceneBase(nullptr, SkinType::COURSE_TRANS, 240)
{
    _type = SceneType::COURSE_TRANS;

    LOG_DEBUG << "[PlayCourseTrans]";
    clearContextPlayForRetry();
    gNextScene = SceneType::PLAY;
}
