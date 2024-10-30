#include "game/runtime/i18n.h"
#include "game/skin/skin_lr2.h"
#include "imgui.h"
#include "scene_play.h"
#include <tuple>

bool ScenePlay::shouldShowImgui() const
{
    return imguiShowAdjustMenu || SceneBase::shouldShowImgui();
}

void ScenePlay::updateImgui()
{
    SceneBase::updateImgui();
    if (gNextScene != SceneType::PLAY)
        return;

    imguiAdjustMenu();
}

void ScenePlay::imguiInit()
{
    std::tie(imguiAdjustBorderX, imguiAdjustBorderY) = pSkin->info.resolution;
    // Arbitrary sane-looking value.
    // floor(640/19)=33px
    // floor(1280/19)=67px
    // floor(1920/19)=101px
    // floor(3840/19)=202px
    static constexpr int noteWidthAdjustmentFactor{19};
    imguiAdjustBorderSize = imguiAdjustBorderX / noteWidthAdjustmentFactor;

    switch (pSkin->info.mode)
    {
    case SkinType::PLAY10:
    case SkinType::PLAY14: imguiAdjustIsDP = true; [[fallthrough]];
    case SkinType::PLAY5_2:
    case SkinType::PLAY7_2:
    case SkinType::PLAY9_2: imguiAdjustHas2P = true; break;
    default: break;
    }
}

void ScenePlay::imguiAdjustMenu()
{
    using namespace i18nText;

    std::shared_ptr<SkinLR2> s = std::dynamic_pointer_cast<SkinLR2>(pSkin);
    if (imguiShowAdjustMenu && s != nullptr)
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

        ImGui::SetNextWindowPos(ImVec2(20.f, 20.f), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(500.f, 0.f), ImGuiCond_Always);
        if (ImGui::Begin("Adjust Menu", &imguiShowAdjustMenu, ImGuiWindowFlags_NoCollapse))
        {
            ImGui::Checkbox(i18n::c(PLAY_SKIN_ADJUST_JUDGE_POS_LIFT), &s->adjustPlayJudgePositionLift);

            ImGui::Dummy({});
            ImGui::Separator();

            if (ImGui::BeginTable("items", 2))
            {
                ImGui::TableSetupColumn("##name", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("##slider", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(i18n::c(PLAY_SKIN_ADJUST_JUDGE_POS_1P_X));
                ImGui::TableNextColumn();
                ImGui::SliderInt("##judge1px", &s->adjustPlayJudgePosition1PX, -imguiAdjustBorderX, imguiAdjustBorderX);
                ImGui::SameLine();
                if (ImGui::Button(" R ##judge1pxreset"))
                    s->adjustPlayJudgePosition1PX = 0;

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(i18n::c(PLAY_SKIN_ADJUST_JUDGE_POS_1P_Y));
                ImGui::TableNextColumn();
                ImGui::SliderInt("##judge1py", &s->adjustPlayJudgePosition1PY, imguiAdjustBorderY, -imguiAdjustBorderY);
                ImGui::SameLine();
                if (ImGui::Button(" R ##judge1pyreset"))
                    s->adjustPlayJudgePosition1PY = 0;

                if (imguiAdjustHas2P)
                {
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(i18n::c(PLAY_SKIN_ADJUST_JUDGE_POS_2P_X));
                    ImGui::TableNextColumn();
                    ImGui::SliderInt("##judge2px", &s->adjustPlayJudgePosition2PX, -imguiAdjustBorderX,
                                     imguiAdjustBorderX);
                    ImGui::SameLine();
                    if (ImGui::Button(" R ##judge2pxreset"))
                        s->adjustPlayJudgePosition2PX = 0;

                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(i18n::c(PLAY_SKIN_ADJUST_JUDGE_POS_2P_Y));
                    ImGui::TableNextColumn();
                    ImGui::SliderInt("##judge2py", &s->adjustPlayJudgePosition2PY, imguiAdjustBorderY,
                                     -imguiAdjustBorderY);
                    ImGui::SameLine();
                    if (ImGui::Button(" R ##judge2pyreset"))
                        s->adjustPlayJudgePosition2PY = 0;
                }

                ImGui::TableNextRow();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(i18n::c(PLAY_SKIN_ADJUST_NOTE_1P_X));
                ImGui::TableNextColumn();
                ImGui::SliderInt("##note1px", &s->adjustPlayNote1PX, -imguiAdjustBorderX, imguiAdjustBorderX);
                ImGui::SameLine();
                if (ImGui::Button(" R ##note1pxreset"))
                    s->adjustPlayNote1PX = 0;

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(i18n::c(PLAY_SKIN_ADJUST_NOTE_1P_Y));
                ImGui::TableNextColumn();
                ImGui::SliderInt("##note1py", &s->adjustPlayNote1PY, imguiAdjustBorderY, -imguiAdjustBorderY);
                ImGui::SameLine();
                if (ImGui::Button(" R ##note1pyreset"))
                    s->adjustPlayNote1PY = 0;

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(i18n::c(PLAY_SKIN_ADJUST_NOTE_1P_W));
                ImGui::TableNextColumn();
                ImGui::SliderInt("##note1pw", &s->adjustPlayNote1PW, -imguiAdjustBorderSize, imguiAdjustBorderSize);
                ImGui::SameLine();
                if (ImGui::Button(" R ##note1pwreset"))
                    s->adjustPlayNote1PW = 0;

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(i18n::c(PLAY_SKIN_ADJUST_NOTE_1P_H));
                ImGui::TableNextColumn();
                ImGui::SliderInt("##note1ph", &s->adjustPlayNote1PH, -imguiAdjustBorderSize, imguiAdjustBorderSize);
                ImGui::SameLine();
                if (ImGui::Button(" R ##note1phreset"))
                    s->adjustPlayNote1PH = 0;

                if (imguiAdjustHas2P)
                {
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(i18n::c(PLAY_SKIN_ADJUST_NOTE_2P_X));
                    ImGui::TableNextColumn();
                    ImGui::SliderInt("##note2px", &s->adjustPlayNote2PX, -imguiAdjustBorderX, imguiAdjustBorderX);
                    ImGui::SameLine();
                    if (ImGui::Button(" R ##note2pxreset"))
                        s->adjustPlayNote2PX = 0;

                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(i18n::c(PLAY_SKIN_ADJUST_NOTE_2P_Y));
                    ImGui::TableNextColumn();
                    ImGui::SliderInt("##note2py", &s->adjustPlayNote2PY, imguiAdjustBorderY, -imguiAdjustBorderY);
                    ImGui::SameLine();
                    if (ImGui::Button(" R ##note2pyreset"))
                        s->adjustPlayNote2PY = 0;

                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(i18n::c(PLAY_SKIN_ADJUST_NOTE_2P_W));
                    ImGui::TableNextColumn();
                    ImGui::SliderInt("##note2pw", &s->adjustPlayNote2PW, -imguiAdjustBorderSize, imguiAdjustBorderSize);
                    ImGui::SameLine();
                    if (ImGui::Button(" R ##jnote2pwreset"))
                        s->adjustPlayNote2PW = 0;

                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(i18n::c(PLAY_SKIN_ADJUST_NOTE_2P_H));
                    ImGui::TableNextColumn();
                    ImGui::SliderInt("##note2ph", &s->adjustPlayNote2PH, -imguiAdjustBorderSize, imguiAdjustBorderSize);
                    ImGui::SameLine();
                    if (ImGui::Button(" R ##note2phreset"))
                        s->adjustPlayNote2PH = 0;
                }

                ImGui::EndTable();
            }

            if (ImGui::Button(i18n::c(PLAY_SKIN_ADJUST_RESET)))
            {
                s->adjustPlaySkinX = 0;
                s->adjustPlaySkinY = 0;
                s->adjustPlaySkinW = 0;
                s->adjustPlaySkinH = 0;
                s->adjustPlayJudgePositionLift = true;
                s->adjustPlayJudgePosition1PX = 0;
                s->adjustPlayJudgePosition1PY = 0;
                s->adjustPlayJudgePosition2PX = 0;
                s->adjustPlayJudgePosition2PY = 0;
                s->adjustPlayNote1PX = 0;
                s->adjustPlayNote1PY = 0;
                s->adjustPlayNote1PW = 0;
                s->adjustPlayNote1PH = 0;
                s->adjustPlayNote2PX = 0;
                s->adjustPlayNote2PY = 0;
                s->adjustPlayNote2PW = 0;
                s->adjustPlayNote2PH = 0;
            }

            ImGui::End();
        }
    }
    else
    {
        imguiShowAdjustMenu = false;
    }
}
