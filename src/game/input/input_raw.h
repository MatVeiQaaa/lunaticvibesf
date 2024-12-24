#pragma once

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <InitGuid.h>
#include <Windows.h>

#include <common/types.h>
#include <common/keymap.h>

#include <vector>

class InputRawInput
{
public:
    struct DeviceMouse
    {
        std::array<lunaticvibes::Time, 5> update = {0};
        std::array<bool, 5> state = {0};
        float wheelVert = 0.f;
        float wheelHorz = 0.f;
        LONG lastX = 0;
        LONG lastY = 0;
    };
    struct DeviceKeyboard
    {
        std::array<lunaticvibes::Time, (size_t)Input::Keyboard::K_COUNT> update = {0};
        std::array<bool, (size_t)Input::Keyboard::K_COUNT> state = {0};
    };
    struct DeviceJoystick
    {
        
    };

protected:
    DeviceMouse _deviceMouse;
    DeviceKeyboard _deviceKeyboard;
    std::vector<DeviceJoystick> _deviceJoysticks;

public:

public:
    InputRawInput();
    virtual ~InputRawInput();

    size_t getJoystickCount() const;

    //const DIMOUSESTATE& getMouseState() const;
    const std::array<bool, (size_t)Input::Keyboard::K_COUNT>& getKeyboardState() const;
    static int LVKeyToScanCode(Input::Keyboard key);
    static Input::Keyboard ScanCodeToLVKey(WORD scanCode);
    const std::array<bool, 5>& getMouseState() const;
    float getMouseZ();
    //const DIJOYSTATE& getJoystickState(size_t idx) const;
    //const DeviceJoystick::Capabilities& getJoystickCapabilities(size_t idx) const;

public:
    static InputRawInput& inst();

public:
    void InputMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    WNDPROC GetPreviousWndProc();

private:
    WNDPROC _prevWndProc = nullptr;
    std::vector<RAWINPUTDEVICELIST> _deviceList;
};

#endif
