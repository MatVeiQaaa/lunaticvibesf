#ifdef _WIN32

#include <common/assert.h>
#include <common/log.h>
#include <common/sysutil.h>
#include <game/input/input_raw.h>

static const std::string GetWinErrorMessage(DWORD dwErr)
{
    CHAR wszMsgBuff[512];
    DWORD dwChars;

    // Try to get the message from the system errors.
    dwChars = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwErr, 0, wszMsgBuff, 512,
                            NULL);
    if (!dwChars)
    {
        // The error code did not exist in the system errors.
        // Try Ntdsbmsg.dll for the error code.
        HINSTANCE hInst;
        hInst = LoadLibrary("Ntdsbmsg.dll");
        if (NULL == hInst)
        {
            LOG_ERROR << "GetWinErrorMessage() couldn't load Ntdsbmsg.dll";
            std::string errorMessage = "UNKNOWN ERROR";
            return errorMessage;
        }
        dwChars = FormatMessage(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS, hInst, dwErr, 0,
                                wszMsgBuff, 512, NULL);
        if (!dwChars)
        {
            std::string errorMessage = "UNKNOWN ERROR";
            return errorMessage;
        }
        FreeLibrary(hInst);
    }
    std::string errorMessage = wszMsgBuff;
    return errorMessage;
}

const static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    InputRawInput::inst().InputMessageHandler(hWnd, msg, wParam, lParam);
    return CallWindowProc(InputRawInput::inst().GetPreviousWndProc(), hWnd, msg, wParam, lParam);
};

InputRawInput::InputRawInput()
{
    HWND hwnd = NULL;
    getWindowHandle(&hwnd);
    _prevWndProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));

    RAWINPUTDEVICE rid[2];

    rid[0].usUsagePage = 0x01;       // HID_USAGE_PAGE_GENERIC
    rid[0].usUsage = 0x02;           // HID_USAGE_GENERIC_MOUSE
    rid[0].dwFlags = RIDEV_NOLEGACY; // adds mouse and also ignores legacy mouse messages
    rid[0].hwndTarget = 0;

    rid[1].usUsagePage = 0x01;       // HID_USAGE_PAGE_GENERIC
    rid[1].usUsage = 0x06;           // HID_USAGE_GENERIC_KEYBOARD
    rid[1].dwFlags = RIDEV_NOLEGACY; // adds keyboard and also ignores legacy keyboard messages
    rid[1].hwndTarget = 0;

    //rid[2].usUsagePage = 0x01; // HID_USAGE_PAGE_GENERIC
    //rid[2].usUsage = 0x05;     // HID_USAGE_GENERIC_GAMEPAD
    //rid[2].dwFlags = 0;        // adds game pad
    //rid[2].hwndTarget = 0;

    //rid[3].usUsagePage = 0x01; // HID_USAGE_PAGE_GENERIC
    //rid[3].usUsage = 0x04;     // HID_USAGE_GENERIC_JOYSTICK
    //rid[3].dwFlags = 0;        // adds joystick
    //rid[3].hwndTarget = 0;

    UINT nDevices = 0;
    if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0)
    {
        DWORD errCode = GetLastError();
        std::string errMsg = GetWinErrorMessage(errCode);
        LOG_ERROR << "[Input] RawInput create error: [" << errCode << "] " << errMsg;
    }
    RAWINPUTDEVICELIST* deviceList = (RAWINPUTDEVICELIST*)malloc(sizeof(RAWINPUTDEVICELIST) * nDevices);
    if (GetRawInputDeviceList(deviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) < 0)
    {
        DWORD errCode = GetLastError();
        std::string errMsg = GetWinErrorMessage(errCode);
        LOG_ERROR << "[Input] RawInput create error: [" << errCode << "] " << errMsg;
    }

    for (int i = 0; i < nDevices; i++)
    {
        _deviceList.push_back(deviceList[i]);
    }
    free(deviceList);

    BOOL res = RegisterRawInputDevices(rid, std::size(rid), sizeof(RAWINPUTDEVICE));
    if (res == FALSE)
    {
        DWORD errCode = GetLastError();
        std::string errMsg = GetWinErrorMessage(errCode);
        LOG_ERROR << "[Input] RawInput create error: [" << errCode << "] " << errMsg;
    }
}

InputRawInput::~InputRawInput()
{
}

void InputRawInput::InputMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg != WM_INPUT)
        return;

    lunaticvibes::Time timestamp;

    UINT dwSize = 0;
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
    RAWINPUT* raw = (RAWINPUT*)new BYTE[dwSize];

    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, raw, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
        LOG_ERROR << "[Input] GetRawInputData does not return correct size!";
    switch (raw->header.dwType)
    {
    case RIM_TYPEKEYBOARD: {
        RAWKEYBOARD& input = raw->data.keyboard;
        if (input.MakeCode == KEYBOARD_OVERRUN_MAKE_CODE || input.VKey >= UCHAR_MAX)
            break;

        if (input.MakeCode)
        {
            WORD scanCode = MAKEWORD(input.MakeCode & 0x7f,
                                     ((input.Flags & RI_KEY_E0) ? 0xe0 : ((input.Flags & RI_KEY_E1) ? 0xe1 : 0x00)));
            deviceKeyboard.state[(size_t)ScanCodeToLVKey(input.MakeCode)] = !(input.Flags & RI_KEY_BREAK);
            deviceKeyboard.update[(size_t)ScanCodeToLVKey(input.MakeCode)] = timestamp;
        }
        break;
    }
    case RIM_TYPEMOUSE: {
        RAWMOUSE& input = raw->data.mouse;
        break;
    }
    case RIM_TYPEHID: {
        RAWHID& input = raw->data.hid;
        break;
    }
    default: {
        LOG_ERROR << "[Input] Unknown RawInput device type received with value " << raw->header.dwType << "!";
        break;
    }
    }
}

size_t InputRawInput::getJoystickCount() const
{
    return deviceJoysticks.size();
}

int InputRawInput::LVKeyToScanCode(Input::Keyboard key)
{
    switch (key)
    {
    case Input::Keyboard::K_ESC: return 1;
    case Input::Keyboard::K_1: return 2;
    case Input::Keyboard::K_2: return 3;
    case Input::Keyboard::K_3: return 4;
    case Input::Keyboard::K_4: return 5;
    case Input::Keyboard::K_5: return 6;
    case Input::Keyboard::K_6: return 7;
    case Input::Keyboard::K_7: return 8;
    case Input::Keyboard::K_8: return 9;
    case Input::Keyboard::K_9: return 10;
    case Input::Keyboard::K_0: return 11;
    case Input::Keyboard::K_MINUS: return 12;
    case Input::Keyboard::K_EQUAL: return 13;
    case Input::Keyboard::K_BKSP: return 14;
    case Input::Keyboard::K_TAB: return 15;
    case Input::Keyboard::K_Q: return 16;
    case Input::Keyboard::K_W: return 17;
    case Input::Keyboard::K_E: return 18;
    case Input::Keyboard::K_R: return 19;
    case Input::Keyboard::K_T: return 20;
    case Input::Keyboard::K_Y: return 21;
    case Input::Keyboard::K_U: return 22;
    case Input::Keyboard::K_I: return 23;
    case Input::Keyboard::K_O: return 24;
    case Input::Keyboard::K_P: return 25;
    case Input::Keyboard::K_LBRACKET: return 26;
    case Input::Keyboard::K_RBRACKET: return 27;
    case Input::Keyboard::K_ENTER: return 28;
    case Input::Keyboard::K_LCTRL: return 29;
    case Input::Keyboard::K_A: return 30;
    case Input::Keyboard::K_S: return 31;
    case Input::Keyboard::K_D: return 32;
    case Input::Keyboard::K_F: return 33;
    case Input::Keyboard::K_G: return 34;
    case Input::Keyboard::K_H: return 35;
    case Input::Keyboard::K_J: return 36;
    case Input::Keyboard::K_K: return 37;
    case Input::Keyboard::K_L: return 38;
    case Input::Keyboard::K_SEMICOLON: return 39;
    // case Input::Keyboard::K_': return 40; // quote?
    // case Input::Keyboard::K_GRAVE: return 41; // ???
    case Input::Keyboard::K_LSHIFT: return 42;
    case Input::Keyboard::K_BACKSLASH: return 43;
    case Input::Keyboard::K_Z: return 44;
    case Input::Keyboard::K_X: return 45;
    case Input::Keyboard::K_C: return 46;
    case Input::Keyboard::K_V: return 47;
    case Input::Keyboard::K_B: return 48;
    case Input::Keyboard::K_N: return 49;
    case Input::Keyboard::K_M: return 50;
    case Input::Keyboard::K_COMMA: return 51;
    case Input::Keyboard::K_DOT: return 52;
    case Input::Keyboard::K_SLASH: return 53;
    case Input::Keyboard::K_RSHIFT: return 54;
    case Input::Keyboard::K_PRTSC: return 55;
    case Input::Keyboard::K_LALT: return 56;
    case Input::Keyboard::K_SPACE: return 57;
    case Input::Keyboard::K_CAPSLOCK: return 58;
    case Input::Keyboard::K_F1: return 59;
    case Input::Keyboard::K_F2: return 60;
    case Input::Keyboard::K_F3: return 61;
    case Input::Keyboard::K_F4: return 62;
    case Input::Keyboard::K_F5: return 63;
    case Input::Keyboard::K_F6: return 64;
    case Input::Keyboard::K_F7: return 65;
    case Input::Keyboard::K_F8: return 66;
    case Input::Keyboard::K_F9: return 67;
    case Input::Keyboard::K_F10: return 68;
    case Input::Keyboard::K_NUMLOCK: return 69;
    case Input::Keyboard::K_SCRLOCK: return 70;
    case Input::Keyboard::K_HOME: return 71;
    case Input::Keyboard::K_UP: return 72;
    case Input::Keyboard::K_PGUP: return 73;
    // case Input::Keyboard::K_MINUS: return 74; // dolbaeb?
    case Input::Keyboard::K_LEFT: return 75;
    // case Input::Keyboard::K_CENTER: return 76;
    case Input::Keyboard::K_RIGHT: return 77;
    // case Input::Keyboard::K_PLUS: return 78;
    case Input::Keyboard::K_END: return 79;
    case Input::Keyboard::K_DOWN: return 80;
    case Input::Keyboard::K_PGDN: return 81;
    case Input::Keyboard::K_INS: return 82;
    case Input::Keyboard::K_DEL: return 83;
    case Input::Keyboard::K_ERROR:
    case Input::Keyboard::K_APOSTROPHE:
    case Input::Keyboard::K_TYPEWRITER_APS:
    case Input::Keyboard::K_NUM7:
    case Input::Keyboard::K_NUM8:
    case Input::Keyboard::K_NUM9:
    case Input::Keyboard::K_NUM_MINUS:
    case Input::Keyboard::K_NUM4:
    case Input::Keyboard::K_NUM5:
    case Input::Keyboard::K_NUM6:
    case Input::Keyboard::K_NUM_PLUS:
    case Input::Keyboard::K_NUM1:
    case Input::Keyboard::K_NUM2:
    case Input::Keyboard::K_NUM3:
    case Input::Keyboard::K_NUM0:
    case Input::Keyboard::K_NUM_DOT:
    case Input::Keyboard::K_SYSRQ:
    case Input::Keyboard::K_F11:
    case Input::Keyboard::K_F12:
    case Input::Keyboard::K_F13:
    case Input::Keyboard::K_F14:
    case Input::Keyboard::K_F15:
    case Input::Keyboard::K_PAUSE:
    case Input::Keyboard::K_RALT:
    case Input::Keyboard::K_RCTRL:
    case Input::Keyboard::K_JP_YEN:
    case Input::Keyboard::K_JP_NOCONVERT:
    case Input::Keyboard::K_JP_CONVERT:
    case Input::Keyboard::K_JP_KANA:
    case Input::Keyboard::K_NONUS_BACKSLASH:
    case Input::Keyboard::K_NUM_SLASH:
    case Input::Keyboard::K_NUM_STAR:
    case Input::Keyboard::K_NUM_ENTER:
    case Input::Keyboard::K_COUNT: return 0;
    }
    lunaticvibes::assert_failed("otsosi");
}

Input::Keyboard InputRawInput::ScanCodeToLVKey(WORD scanCode)
{
    switch (scanCode)
    {
    case 1: return Input::Keyboard::K_ESC;
    case 2: return Input::Keyboard::K_1;
    case 3: return Input::Keyboard::K_2;
    case 4: return Input::Keyboard::K_3;
    case 5: return Input::Keyboard::K_4;
    case 6: return Input::Keyboard::K_5;
    case 7: return Input::Keyboard::K_6;
    case 8: return Input::Keyboard::K_7;
    case 9: return Input::Keyboard::K_8;
    case 10: return Input::Keyboard::K_9;
    case 11: return Input::Keyboard::K_0;
    case 12: return Input::Keyboard::K_MINUS;
    case 13: return Input::Keyboard::K_EQUAL;
    case 14: return Input::Keyboard::K_BKSP;
    case 15: return Input::Keyboard::K_TAB;
    case 16: return Input::Keyboard::K_Q;
    case 17: return Input::Keyboard::K_W;
    case 18: return Input::Keyboard::K_E;
    case 19: return Input::Keyboard::K_R;
    case 20: return Input::Keyboard::K_T;
    case 21: return Input::Keyboard::K_Y;
    case 22: return Input::Keyboard::K_U;
    case 23: return Input::Keyboard::K_I;
    case 24: return Input::Keyboard::K_O;
    case 25: return Input::Keyboard::K_P;
    case 26: return Input::Keyboard::K_LBRACKET;
    case 27: return Input::Keyboard::K_RBRACKET;
    case 28: return Input::Keyboard::K_ENTER;
    case 29: return Input::Keyboard::K_LCTRL;
    case 30: return Input::Keyboard::K_A;
    case 31: return Input::Keyboard::K_S;
    case 32: return Input::Keyboard::K_D;
    case 33: return Input::Keyboard::K_F;
    case 34: return Input::Keyboard::K_G;
    case 35: return Input::Keyboard::K_H;
    case 36: return Input::Keyboard::K_J;
    case 37: return Input::Keyboard::K_K;
    case 38: return Input::Keyboard::K_L;
    case 39: return Input::Keyboard::K_SEMICOLON;
    // case 40: return Input::Keyboard::K_'; // quote?
    // case 41: return Input::Keyboard::K_GRAVE; // ???
    case 42: return Input::Keyboard::K_LSHIFT;
    case 43: return Input::Keyboard::K_BACKSLASH;
    case 44: return Input::Keyboard::K_Z;
    case 45: return Input::Keyboard::K_X;
    case 46: return Input::Keyboard::K_C;
    case 47: return Input::Keyboard::K_V;
    case 48: return Input::Keyboard::K_B;
    case 49: return Input::Keyboard::K_N;
    case 50: return Input::Keyboard::K_M;
    case 51: return Input::Keyboard::K_COMMA;
    case 52: return Input::Keyboard::K_DOT;
    case 53: return Input::Keyboard::K_SLASH;
    case 54: return Input::Keyboard::K_RSHIFT;
    case 55: return Input::Keyboard::K_PRTSC;
    case 56: return Input::Keyboard::K_LALT;
    case 57: return Input::Keyboard::K_SPACE;
    case 58: return Input::Keyboard::K_CAPSLOCK;
    case 59: return Input::Keyboard::K_F1;
    case 60: return Input::Keyboard::K_F2;
    case 61: return Input::Keyboard::K_F3;
    case 62: return Input::Keyboard::K_F4;
    case 63: return Input::Keyboard::K_F5;
    case 64: return Input::Keyboard::K_F6;
    case 65: return Input::Keyboard::K_F7;
    case 66: return Input::Keyboard::K_F8;
    case 67: return Input::Keyboard::K_F9;
    case 68: return Input::Keyboard::K_F10;
    case 69: return Input::Keyboard::K_NUMLOCK;
    case 70: return Input::Keyboard::K_SCRLOCK;
    case 71: return Input::Keyboard::K_HOME;
    case 72: return Input::Keyboard::K_UP;
    case 73: return Input::Keyboard::K_PGUP;
    // case 74: return Input::Keyboard::K_MINUS; // dolbaeb?
    case 75: return Input::Keyboard::K_LEFT;
    // case 76: return Input::Keyboard::K_CENTER;
    case 77: return Input::Keyboard::K_RIGHT;
    // case 78: return Input::Keyboard::K_PLUS;
    case 79: return Input::Keyboard::K_END;
    case 80: return Input::Keyboard::K_DOWN;
    case 81: return Input::Keyboard::K_PGDN;
    case 82: return Input::Keyboard::K_INS;
    case 83: return Input::Keyboard::K_DEL;
    case 0:
    default: return Input::Keyboard::K_ERROR; // pizda
    }
}


const std::array<bool, (size_t)Input::Keyboard::K_COUNT>& InputRawInput::getKeyboardState() const
{
    return deviceKeyboard.state;
}

InputRawInput& InputRawInput::inst()
{
    static InputRawInput _inst;
    return _inst;
}

WNDPROC InputRawInput::GetPreviousWndProc()
{
    return _prevWndProc;
}

#endif
