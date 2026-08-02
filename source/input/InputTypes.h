#pragma once
#include <cstdint>
#include "../math_custom/Vector2.h"


enum class InputAction : uint8_t { Released, Pressed };

enum class KeyCode : uint16_t {
    Unknown = 0,
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    Space, Enter, Escape, Tab, Backspace, Delete,
    Left, Right, Up, Down,
    LeftShift, RightShift, LeftControl, RightControl, LeftAlt, RightAlt,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    Count 
};

enum class MouseButton : uint8_t {
    Left = 0, Right, Middle,
    Count
};


struct KeyEvent          { KeyCode key; InputAction action; };
struct MouseButtonEvent  { MouseButton button; InputAction action; };
struct MouseMoveEvent    { double x, y; };
struct ScrollEvent       { double xoffset, yoffset; };

struct TouchEvent        { int id; Vector2 pos; InputAction action; };
struct TouchMoveEvent    { int id; Vector2 pos; };