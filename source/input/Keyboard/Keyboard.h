#pragma once
#include <array>
#include "input/InputTypes.h"

class InputManager;

class Keyboard {
public:
    bool IsDown(KeyCode key) const      { return current[Index(key)]; }
    bool WasPressed(KeyCode key) const  { return pressed[Index(key)]; }
    bool WasReleased(KeyCode key) const { return released[Index(key)]; }

private:
    friend class InputManager;
    void OnKeyEvent(const KeyEvent& e);
    void EndFrame();

    static constexpr size_t Index(KeyCode k) { return static_cast<size_t>(k); }

    std::array<bool, static_cast<size_t>(KeyCode::Count)> current{};
    std::array<bool, static_cast<size_t>(KeyCode::Count)> pressed{};
    std::array<bool, static_cast<size_t>(KeyCode::Count)> released{};
};