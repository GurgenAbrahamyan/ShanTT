#include "InputManager.h"
#include "core/EventBus.h"

InputManager::InputManager(EventBus& bus)
{
    bus.subscribe<KeyEvent>(this, [this](const KeyEvent& e) { keyboard.OnKeyEvent(e); });
    bus.subscribe<MouseButtonEvent>(this, [this](const MouseButtonEvent& e) { mouse.OnButtonEvent(e); });
    bus.subscribe<MouseMoveEvent>(this, [this](const MouseMoveEvent& e) { mouse.OnMoveEvent(e); });
    bus.subscribe<ScrollEvent>(this, [this](const ScrollEvent& e) { mouse.OnScrollEvent(e); });
}

void InputManager::EndFrame()
{
    keyboard.EndFrame();
    mouse.EndFrame();
}