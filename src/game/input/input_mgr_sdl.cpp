#ifndef _WIN32
#ifdef RENDER_SDL2

#include <algorithm>
#include <array>

#include <SDL.h>

#include "common/log.h"
#include "game/graphics/SDL2/input.h"

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
	SdlInputHolder& operator=(SdlInputHolder &&) = delete;
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
	LOG_INFO << "[SDL2] Found " << joystick_count
			 << " joysticks. Please note they are not fully supported with SDL2 backend.";
	for (int i = 0; i < std::min(joystick_count, static_cast<int>(sdl::state::g_joysticks.size())); ++i)
	{
		state.joysticks[i] = SDL_JoystickOpen(i);
		if (state.joysticks[i] == nullptr)
		{
			LOG_ERROR << "[SDL2] Failed to open joystick: " << SDL_GetError();
			continue;
		}
		ret = SDL_JoystickNumButtons(state.joysticks[i]);
		if (ret < 0)
		{
			LOG_ERROR << "[SDL2] SDL_JoystickNumButtons failed: " << SDL_GetError();
		} else {
			LOG_INFO << "[SDL2] Joystick " << i << " has " << ret << " buttons, our limit is "
					 << sdl::state::g_joysticks[i].size();
		}
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
	assert(c.device < sdl::state::g_joysticks.size());
	const auto& buttons = sdl::state::g_joysticks[c.device];
	if (c.type == Input::Joystick::Type::BUTTON)
	{
		if (buttons.size() < c.index)
			return false;
		return buttons[c.index];
	}
	return false;
}
double getJoystickAxis(size_t device, Input::Joystick::Type type, size_t index)
{
	// TODO
	return 0.;
}

bool isMouseButtonPressed(int idx)
{
	// Lunaticvibes expects middle and right mouse buttons to be swapped.
	switch (idx) {
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
