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
    DeviceMouse deviceMouse;
    DeviceKeyboard deviceKeyboard;
    std::vector<DeviceJoystick> deviceJoysticks;

public:

public:
    InputRawInput();
    virtual ~InputRawInput();

    size_t getJoystickCount() const;

    //const DIMOUSESTATE& getMouseState() const;
    const std::array<bool, (size_t)Input::Keyboard::K_COUNT>& getKeyboardState() const;
    static int LVKeyToScanCode(Input::Keyboard key);
    static Input::Keyboard ScanCodeToLVKey(WORD scanCode);
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
