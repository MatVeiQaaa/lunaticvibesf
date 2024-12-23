#ifndef _WIN32
#ifdef RENDER_SDL2

#include <common/assert.h>
#include <common/log.h>
#include <common/types.h>
#include <game/graphics/SDL2/input.h>
#include <game/input/input_mgr.h>

#include <algorithm>
#include <array>

#include <SDL.h>

struct SdlJoystickDeleter
{
    void operator()(SDL_Joystick* joystick)
    {
        if (joystick != nullptr)
            SDL_JoystickClose(joystick);
    }
};
using SdlJoystickPtr = std::unique_ptr<SDL_Joystick, SdlJoystickDeleter>;

struct SdlInputHolder
{
    std::array<SDL_Joystick*, InputMgr::MAX_JOYSTICK_COUNT> joysticks;
    SdlInputHolder();
    SdlInputHolder(const SdlInputHolder&) = delete;
    SdlInputHolder(SdlInputHolder&&) = delete;
    SdlInputHolder& operator=(const SdlInputHolder&) = delete;
    SdlInputHolder& operator=(SdlInputHolder&&) = delete;
    ~SdlInputHolder();
};

SdlInputHolder::SdlInputHolder()
{
    LOG_INFO << "[SDL2] Initializing Joystick";
    SDL_Init(SDL_INIT_JOYSTICK);
}

SdlInputHolder::~SdlInputHolder()
{
    joysticks.fill(nullptr);
    LOG_INFO << "[SDL2] De-initializing Joystick";
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

[[nodiscard]] static SdlInputHolder& getInputState()
{
    static SdlInputHolder state;
    return state;
}

void initInput()
{
    LOG_WARNING << "[SDL2] Direct input isn't available, input precision is limited by draw FPS";
    (void)getInputState();
}

inline SDL_Joystick* g_game_controller;

void refreshInputDevices()
{
    auto& state = getInputState();
    int ret = SDL_NumJoysticks();
    if (ret < 0)
    {
        LOG_ERROR << "[SDL2] SDL_NumJoysticks failed: " << SDL_GetError();
        return;
    }
    if (ret < 1)
    {
        LOG_INFO << "[SDL2] No joystick connected";
        return;
    }
    const int joystick_count = ret;
    LOG_INFO << "[SDL2] Found " << joystick_count << " joysticks";
    for (int i = 0; i < std::min(joystick_count, static_cast<int>(sdl::state::g_joysticks.size())); ++i)
    {
        LOG_DEBUG << "[SDL2] Opening joystick " << i;
        auto j = SDL_JoystickOpen(i);
        if (j == nullptr)
        {
            LOG_ERROR << "[SDL2] Failed to open joystick: " << SDL_GetError();
            continue;
        }

        ret = SDL_JoystickNumButtons(j);
        if (ret < 0)
        {
            LOG_ERROR << "[SDL2] SDL_JoystickNumButtons failed: " << SDL_GetError();
            continue;
        }
        const auto buttons = static_cast<unsigned int>(ret);

        ret = SDL_JoystickNumAxes(j);
        if (ret < 0)
        {
            LOG_ERROR << "[SDL2] SDL_JoystickNumAxes failed: " << SDL_GetError();
            continue;
        }
        const auto axes = static_cast<unsigned int>(ret);

        ret = SDL_JoystickNumHats(j);
        if (ret < 0)
        {
            LOG_ERROR << "[SDL2] SDL_JoystickNumHats failed: " << SDL_GetError();
            continue;
        }
        const auto hats = static_cast<unsigned int>(ret);

        const char* name = SDL_JoystickName(j);
        if (name == nullptr)
            name = "(unnamed)";

        std::array<char, 64> guid{0};
        SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(j), guid.data(), guid.size());

        state.joysticks[i] = j;

        LOG_INFO << "[SDL2] Initialized joystick: " << name;
        LOG_DEBUG << "[SDL2] GUID: " << guid.data();
        LOG_DEBUG << "[SDL2] Using up to " << sdl::state::g_joysticks[i].size() << "/" << buttons << " buttons";
        LOG_DEBUG << "[SDL2] Using up to " << sdl::state::g_joystick_axes[i].size() << "/" << axes << " axes";
        LOG_DEBUG << "[SDL2] Using up to " << 0 << "/" << hats << " hats";
    }
}

void pollInput()
{
    // No-op.
    // We don't need to do anything, since all the updating is done by
    // window events.
}

bool isKeyPressed(Input::Keyboard key)
{
    return sdl::state::g_keyboard_scancodes[sdl_key_from_common_scancode(key)];
}

bool isButtonPressed(Input::Joystick c, double deadzone)
{
    switch (c.type)
    {
    case Input::Joystick::Type::BUTTON: {
        LVF_DEBUG_ASSERT(c.device < sdl::state::g_joysticks.size());
        const auto& buttons = sdl::state::g_joysticks[c.device];
        if (buttons.size() < c.index)
            return false;
        return buttons[c.index];
    }
    case Input::Joystick::Type::AXIS_RELATIVE_POSITIVE:
    case Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE: return getJoystickAxis(c.device, c.type, c.index) >= deadzone;
    case Input::Joystick::Type::POV: return false; // Hat in SDL terminology. ENOTSUP.
    case Input::Joystick::Type::UNDEF:
    case Input::Joystick::Type::AXIS_ABSOLUTE: break;
    }
    lunaticvibes::verify_failed("isButtonPressed");
    return false;
}

double getJoystickAxis(size_t device, Input::Joystick::Type type, size_t index)
{
    // 1. Axis conversions are broken.
    // 2. Gamepad triggers also send button events in addition to axis events. Wtf?
    /*
    LVF_DEBUG_ASSERT(device < sdl::state::g_joystick_axes.size());
    const auto& axes = sdl::state::g_joystick_axes[device];
    if (axes.size() < index)
        return -1.;
    const auto axis = static_cast<uint16_t>(axes[index]);
    if (axis == 0)
        return -1.;
    switch (type)
    {
    case Input::Joystick::Type::UNDEF:
    case Input::Joystick::Type::BUTTON:
    case Input::Joystick::Type::POV: break;
    case Input::Joystick::Type::AXIS_RELATIVE_POSITIVE: return (axis - 32767) / -32767.0;
    case Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE: return (axis - 32767) / 32767.0;
    case Input::Joystick::Type::AXIS_ABSOLUTE: return axis / 65535.;
    }
    lunaticvibes::verify_failed("getJoystickAxis");
    */
    return -1.;
}

bool isMouseButtonPressed(int idx)
{
    // Lunaticvibes expects middle and right mouse buttons to be swapped.
    switch (idx)
    {
    case 2: idx = 3; break;
    case 3: idx = 2; break;
    default: break;
    }

    return sdl::state::g_mouse_buttons[idx];
}

short getLastMouseWheelState()
{
    auto state = sdl::state::g_mouse_wheel_delta;
    sdl::state::g_mouse_wheel_delta = 0;
    return state;
}

#endif // RENDER_SDL2
#endif // _WIN32
