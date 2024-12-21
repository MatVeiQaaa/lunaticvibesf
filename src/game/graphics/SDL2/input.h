#include <common/keymap.h>
#include <game/input/input_mgr.h>

#include <array>
#include <cstdint>

#include <SDL.h>

using int16_t = std::int16_t;

namespace sdl::state
{

inline constexpr size_t SDL_MOUSE_BUTTON_COUNT = 256;

// `true` for keys currently being pressed down.
inline bool g_keyboard_scancodes[SDL_NUM_SCANCODES];
// `true` for buttons currently being pressed down.
inline bool g_mouse_buttons[SDL_MOUSE_BUTTON_COUNT];
inline int g_mouse_x = 0;
inline int g_mouse_y = 0;
// Reset this to `0` after use.
inline short g_mouse_wheel_delta = 0;
inline std::array<std::array<int16_t, InputMgr::MAX_JOYSTICK_AXIS_COUNT>, InputMgr::MAX_JOYSTICK_COUNT>
    g_joystick_axes{};
// `true` for buttons currently being pressed down.
inline std::array<std::array<bool, InputMgr::MAX_JOYSTICK_BUTTON_COUNT>, InputMgr::MAX_JOYSTICK_COUNT> g_joysticks{};

} // namespace sdl::state

unsigned char sdl_key_from_common_scancode(Input::Keyboard key);
