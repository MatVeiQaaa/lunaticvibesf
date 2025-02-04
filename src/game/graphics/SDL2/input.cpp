#include "input.h"

#include <common/assert.h>

#include <SDL_scancode.h>

unsigned char sdl_key_from_common_scancode(Input::Keyboard key)
{
    // Enum members in order, hopefully the compiler will make a nice jump table out of this.
    switch (key)
    {
    case Input::Keyboard::K_ERROR: return SDL_SCANCODE_UNKNOWN;
    case Input::Keyboard::K_ESC: return SDL_SCANCODE_ESCAPE;
    case Input::Keyboard::K_1: return SDL_SCANCODE_1;
    case Input::Keyboard::K_2: return SDL_SCANCODE_2;
    case Input::Keyboard::K_3: return SDL_SCANCODE_3;
    case Input::Keyboard::K_4: return SDL_SCANCODE_4;
    case Input::Keyboard::K_5: return SDL_SCANCODE_5;
    case Input::Keyboard::K_6: return SDL_SCANCODE_6;
    case Input::Keyboard::K_7: return SDL_SCANCODE_7;
    case Input::Keyboard::K_8: return SDL_SCANCODE_8;
    case Input::Keyboard::K_9: return SDL_SCANCODE_9;
    case Input::Keyboard::K_0: return SDL_SCANCODE_0;
    case Input::Keyboard::K_MINUS: return SDL_SCANCODE_MINUS;
    case Input::Keyboard::K_EQUAL: return SDL_SCANCODE_EQUALS;
    case Input::Keyboard::K_BKSP: return SDL_SCANCODE_BACKSPACE;
    case Input::Keyboard::K_TAB: return SDL_SCANCODE_TAB;
    case Input::Keyboard::K_Q: return SDL_SCANCODE_Q;
    case Input::Keyboard::K_W: return SDL_SCANCODE_W;
    case Input::Keyboard::K_E: return SDL_SCANCODE_E;
    case Input::Keyboard::K_R: return SDL_SCANCODE_R;
    case Input::Keyboard::K_T: return SDL_SCANCODE_T;
    case Input::Keyboard::K_Y: return SDL_SCANCODE_Y;
    case Input::Keyboard::K_U: return SDL_SCANCODE_U;
    case Input::Keyboard::K_I: return SDL_SCANCODE_I;
    case Input::Keyboard::K_O: return SDL_SCANCODE_O;
    case Input::Keyboard::K_P: return SDL_SCANCODE_P;
    case Input::Keyboard::K_LBRACKET: return SDL_SCANCODE_LEFTBRACKET;
    case Input::Keyboard::K_RBRACKET: return SDL_SCANCODE_RIGHTBRACKET;
    case Input::Keyboard::K_ENTER: return SDL_SCANCODE_RETURN;
    case Input::Keyboard::K_LCTRL: return SDL_SCANCODE_LCTRL;
    case Input::Keyboard::K_A: return SDL_SCANCODE_A;
    case Input::Keyboard::K_S: return SDL_SCANCODE_S;
    case Input::Keyboard::K_D: return SDL_SCANCODE_D;
    case Input::Keyboard::K_F: return SDL_SCANCODE_F;
    case Input::Keyboard::K_G: return SDL_SCANCODE_G;
    case Input::Keyboard::K_H: return SDL_SCANCODE_H;
    case Input::Keyboard::K_J: return SDL_SCANCODE_J;
    case Input::Keyboard::K_K: return SDL_SCANCODE_K;
    case Input::Keyboard::K_L: return SDL_SCANCODE_L;
    case Input::Keyboard::K_SEMICOLON: return SDL_SCANCODE_SEMICOLON;
    case Input::Keyboard::K_APOSTROPHE: return SDL_SCANCODE_APOSTROPHE;
    case Input::Keyboard::K_TYPEWRITER_APS: return SDL_SCANCODE_GRAVE;
    case Input::Keyboard::K_LSHIFT: return SDL_SCANCODE_LSHIFT;
    case Input::Keyboard::K_BACKSLASH: return SDL_SCANCODE_BACKSLASH;
    case Input::Keyboard::K_Z: return SDL_SCANCODE_Z;
    case Input::Keyboard::K_X: return SDL_SCANCODE_X;
    case Input::Keyboard::K_C: return SDL_SCANCODE_C;
    case Input::Keyboard::K_V: return SDL_SCANCODE_V;
    case Input::Keyboard::K_B: return SDL_SCANCODE_B;
    case Input::Keyboard::K_N: return SDL_SCANCODE_N;
    case Input::Keyboard::K_M: return SDL_SCANCODE_M;
    case Input::Keyboard::K_COMMA: return SDL_SCANCODE_COMMA;
    case Input::Keyboard::K_DOT: return SDL_SCANCODE_PERIOD;
    case Input::Keyboard::K_SLASH: return SDL_SCANCODE_SLASH;
    case Input::Keyboard::K_RSHIFT: return SDL_SCANCODE_RSHIFT;
    case Input::Keyboard::K_PRTSC: return SDL_SCANCODE_PRINTSCREEN;
    case Input::Keyboard::K_LALT: return SDL_SCANCODE_LALT;
    case Input::Keyboard::K_SPACE: return SDL_SCANCODE_SPACE;
    case Input::Keyboard::K_CAPSLOCK: return SDL_SCANCODE_CAPSLOCK;
    case Input::Keyboard::K_F1: return SDL_SCANCODE_F1;
    case Input::Keyboard::K_F2: return SDL_SCANCODE_F2;
    case Input::Keyboard::K_F3: return SDL_SCANCODE_F3;
    case Input::Keyboard::K_F4: return SDL_SCANCODE_F4;
    case Input::Keyboard::K_F5: return SDL_SCANCODE_F5;
    case Input::Keyboard::K_F6: return SDL_SCANCODE_F6;
    case Input::Keyboard::K_F7: return SDL_SCANCODE_F7;
    case Input::Keyboard::K_F8: return SDL_SCANCODE_F8;
    case Input::Keyboard::K_F9: return SDL_SCANCODE_F9;
    case Input::Keyboard::K_F10: return SDL_SCANCODE_F10;
    case Input::Keyboard::K_NUMLOCK: return SDL_SCANCODE_NUMLOCKCLEAR;
    case Input::Keyboard::K_SCRLOCK: return SDL_SCANCODE_SCROLLLOCK;
    case Input::Keyboard::K_NUM7: return SDL_SCANCODE_KP_7;
    case Input::Keyboard::K_NUM8: return SDL_SCANCODE_KP_8;
    case Input::Keyboard::K_NUM9: return SDL_SCANCODE_KP_9;
    case Input::Keyboard::K_NUM_MINUS: return SDL_SCANCODE_KP_MINUS;
    case Input::Keyboard::K_NUM4: return SDL_SCANCODE_KP_4;
    case Input::Keyboard::K_NUM5: return SDL_SCANCODE_KP_5;
    case Input::Keyboard::K_NUM6: return SDL_SCANCODE_KP_6;
    case Input::Keyboard::K_NUM_PLUS: return SDL_SCANCODE_KP_PLUS;
    case Input::Keyboard::K_NUM1: return SDL_SCANCODE_KP_1;
    case Input::Keyboard::K_NUM2: return SDL_SCANCODE_KP_2;
    case Input::Keyboard::K_NUM3: return SDL_SCANCODE_KP_3;
    case Input::Keyboard::K_NUM0: return SDL_SCANCODE_KP_0;
    case Input::Keyboard::K_NUM_DOT: return SDL_SCANCODE_KP_PERIOD;
    case Input::Keyboard::K_SYSRQ: return SDL_SCANCODE_SYSREQ;
    case Input::Keyboard::K_F11: return SDL_SCANCODE_F11;
    case Input::Keyboard::K_F12: return SDL_SCANCODE_F12;
    case Input::Keyboard::K_F13: return SDL_SCANCODE_F13;
    case Input::Keyboard::K_F14: return SDL_SCANCODE_F14;
    case Input::Keyboard::K_F15: return SDL_SCANCODE_F15;
    case Input::Keyboard::K_PAUSE: return SDL_SCANCODE_PAUSE;
    case Input::Keyboard::K_INS: return SDL_SCANCODE_INSERT;
    case Input::Keyboard::K_DEL: return SDL_SCANCODE_DELETE;
    case Input::Keyboard::K_HOME: return SDL_SCANCODE_HOME;
    case Input::Keyboard::K_END: return SDL_SCANCODE_END;
    case Input::Keyboard::K_PGUP: return SDL_SCANCODE_PAGEUP;
    case Input::Keyboard::K_PGDN: return SDL_SCANCODE_PAGEDOWN;
    case Input::Keyboard::K_RALT: return SDL_SCANCODE_RALT;
    case Input::Keyboard::K_RCTRL: return SDL_SCANCODE_RCTRL;
    case Input::Keyboard::K_LEFT: return SDL_SCANCODE_LEFT;
    case Input::Keyboard::K_UP: return SDL_SCANCODE_UP;
    case Input::Keyboard::K_RIGHT: return SDL_SCANCODE_RIGHT;
    case Input::Keyboard::K_DOWN: return SDL_SCANCODE_DOWN;
    case Input::Keyboard::K_JP_YEN: return SDL_SCANCODE_INTERNATIONAL3;
    case Input::Keyboard::K_JP_NOCONVERT: return SDL_SCANCODE_INTERNATIONAL5;
    case Input::Keyboard::K_JP_CONVERT: return SDL_SCANCODE_INTERNATIONAL4;
    case Input::Keyboard::K_JP_KANA: return SDL_SCANCODE_INTERNATIONAL2;
    case Input::Keyboard::K_NUM_SLASH: return SDL_SCANCODE_KP_DIVIDE;
    case Input::Keyboard::K_NUM_STAR: return SDL_SCANCODE_KP_MULTIPLY;
    case Input::Keyboard::K_NUM_ENTER: return SDL_SCANCODE_KP_EQUALS;
    case Input::Keyboard::K_NONUS_BACKSLASH: return SDL_SCANCODE_NONUSBACKSLASH;
    case Input::Keyboard::K_COUNT: break;
    }
    LVF_VERIFY(false && "key is SDL_SCANCODE_UNKNOWN");
    return SDL_SCANCODE_UNKNOWN;
}
