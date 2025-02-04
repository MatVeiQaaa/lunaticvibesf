find_package(PkgConfig REQUIRED)
if(${GRAPHICS_BACKEND} STREQUAL SDL2)
    pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2>=2.30.1)
    pkg_check_modules(SDL2_gfx IMPORTED_TARGET SDL2_gfx>=1.0.2)
    if(SDL2_gfx_FOUND)
        set(LVF_SDL2_GFX_TARGET PkgConfig::SDL2_gfx)
    else()
        # vcpkg doesn't make pkg-config file for SDL2_gfx.
        find_package(sdl2-gfx CONFIG REQUIRED)
        set(LVF_SDL2_GFX_TARGET SDL2::SDL2_gfx)
    endif()
    pkg_check_modules(SDL2_image REQUIRED IMPORTED_TARGET SDL2_image>=2.8.2)
    pkg_check_modules(SDL2_ttf REQUIRED IMPORTED_TARGET SDL2_ttf>=2.22.0)
endif()
pkg_check_modules(libavcodec REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(libavformat REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(libavutil REQUIRED IMPORTED_TARGET libavutil)
find_package(CURL REQUIRED)
find_package(cereal 1.3.2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)

if(${GRAPHICS_BACKEND} STREQUAL SDL2)
    set(GRAPHICS_BACKEND_SRC
        graphics/SDL2/common_SDL2.cpp
        graphics/SDL2/input.cpp
        graphics/SDL2/rect_SDL2.cpp
        graphics/SDL2/ttf_SDL2.cpp
        graphics/SDL2/window_SDL2.cpp
    )
endif()

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/git_version.cpp ${CMAKE_BINARY_DIR}/_dummy.cpp
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/git_version.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_library(
    gamelib
    ${CMAKE_BINARY_DIR}/git_version.cpp
    runtime/state.cpp
    chart/chart.cpp
    chart/chart_bms.cpp
    ${GRAPHICS_BACKEND_SRC}
    graphics/sprite.cpp
    graphics/sprite_bar_entry.cpp
    graphics/sprite_graph.cpp
    graphics/sprite_imagetext.cpp
    graphics/sprite_lane.cpp
    graphics/sprite_video.cpp
    graphics/texture_extra.cpp
    graphics/video.cpp
    graphics/dxa.cpp
    input/input_mgr.cpp
    input/input_mgr_sdl.cpp
    input/input_dinput8.cpp
    input/input_windows.cpp
    input/input_wrapper.cpp
    ruleset/ruleset_bms.cpp
    ruleset/ruleset_bms_auto.cpp
    ruleset/ruleset_bms_replay.cpp
    ruleset/ruleset_network.cpp
    ruleset/ruleset_bms_network.cpp
    scene/scene.cpp
    scene/scene_context.cpp
    scene/scene_decide.cpp
    scene/scene_mgr.cpp
    scene/scene_play.cpp
    scene/scene_pre_select.cpp
    scene/scene_result.cpp
    scene/scene_course_result.cpp
    scene/scene_select.cpp
    scene/scene_keyconfig.cpp
    scene/scene_play_course_trans.cpp
    scene/scene_customize.cpp
    scene/scene_select_imgui.cpp
    scene/scene_play_retry_trans.cpp
    scene/scene_exit_trans.cpp
    scene/scene_play_imgui.cpp
    skin/skin.cpp
    skin/skin_lr2.cpp
    skin/skin_lr2_button_callbacks.cpp
    skin/skin_lr2_slider_callbacks.cpp
    skin/skin_lr2_converters.cpp
    skin/skin_lr2_dst.cpp
    skin/skin_lr2_debug.cpp
    skin/skin_mgr.cpp
    sound/sound_fmod.cpp
    sound/sound_fmod_callback.cpp
    sound/sound_mgr.cpp
    sound/sound_sample.cpp
    sound/soundset.cpp
    sound/soundset_lr2.cpp
    replay/replay_chart.cpp
    runtime/i18n.cpp
    arena/arena_data.cpp
    arena/arena_internal.cpp
    arena/arena_host.cpp
    arena/arena_client.cpp
)

target_include_directories(gamelib PUBLIC ${PROJECT_INCLUDE_DIR})

set_target_properties(gamelib PROPERTIES CXX_STANDARD 20)

target_link_libraries(
    gamelib
    PUBLIC cereal::cereal
    PUBLIC common
    PUBLIC config
    PUBLIC CURL::libcurl
    PUBLIC db
    PUBLIC fmod
    PUBLIC imgui::imgui
    PUBLIC PkgConfig::libavcodec
    PUBLIC PkgConfig::libavformat
    PUBLIC PkgConfig::libavutil
    PUBLIC re2::re2
    PRIVATE tinyfiledialogs
)

if(${GRAPHICS_BACKEND} STREQUAL SDL2)
    target_link_libraries(
        gamelib
        PUBLIC ${LVF_SDL2_GFX_TARGET}
        PUBLIC PkgConfig::SDL2
        PUBLIC PkgConfig::SDL2_image
        PUBLIC PkgConfig::SDL2_ttf
        PRIVATE
            $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> # vcpkg
    )
endif()

target_precompile_headers(gamelib REUSE_FROM common)

#############################################################

if(WIN32)
    add_executable(LunaticVibesF game.cpp LunaticVibesF.exe.manifest)
else()
    add_executable(LunaticVibesF game.cpp)
endif()
target_link_libraries(LunaticVibesF PRIVATE gamelib PRIVATE tinyfiledialogs)
set_target_properties(LunaticVibesF PROPERTIES CXX_STANDARD 20)
install(TARGETS LunaticVibesF COMPONENT Game)

add_custom_command(
    TARGET LunaticVibesF
    POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:fmod>
        $<TARGET_FILE_DIR:LunaticVibesF>
)
install(IMPORTED_RUNTIME_ARTIFACTS fmod COMPONENT Game LIBRARY DESTINATION bin)

if(MSVC)
    install(
        FILES $<TARGET_PDB_FILE:LunaticVibesF>
        DESTINATION bin
        COMPONENT Symbols
    )
endif()

add_custom_command(
    TARGET LunaticVibesF
    POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:LunaticVibesF>/gamedata/resources
)
# NOTE: trailing slash is significant.
install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/res/
    DESTINATION bin/gamedata/resources
    COMPONENT Game
)

add_custom_command(
    TARGET LunaticVibesF
    POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/LICENSE_3RD_PARTY
        $<TARGET_FILE_DIR:LunaticVibesF>/gamedata/resources/LICENSE_3RD_PARTY
)
install(
    FILES ${CMAKE_SOURCE_DIR}/LICENSE_3RD_PARTY
    DESTINATION bin
    COMPONENT Game
)
