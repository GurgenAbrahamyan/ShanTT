#pragma once
#include <array>
#include "input/InputTypes.h"
#include "math_custom/Vector2.h"

class InputManager;

class Mouse {
public:
    bool IsDown(MouseButton btn) const      { return current[Index(btn)]; }
    bool WasPressed(MouseButton btn) const  { return pressed[Index(btn)]; }
    bool WasReleased(MouseButton btn) const { return released[Index(btn)]; }

    Vector2 Position() const    { return pos; }
    Vector2 Delta() const       { return pos - lastPos; }
    Vector2 ScrollDelta() const { return scroll; }

private:
    friend class InputManager;
    void OnButtonEvent(const MouseButtonEvent& e);
    void OnMoveEvent(const MouseMoveEvent& e);
    void OnScrollEvent(const ScrollEvent& e);
    void EndFrame();

    static constexpr size_t Index(MouseButton b) { return static_cast<size_t>(b); }

    std::array<bool, static_cast<size_t>(MouseButton::Count)> current{};
    std::array<bool, static_cast<size_t>(MouseButton::Count)> pressed{};
    std::array<bool, static_cast<size_t>(MouseButton::Count)> released{};

    Vector2 pos{};
    Vector2 lastPos{};
    Vector2 scroll{};
};