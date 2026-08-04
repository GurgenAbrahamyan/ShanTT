#pragma once
#include "Keyboard/Keyboard.h"
#include "Mouse/Mouse.h"


class Touch{};
class EventBus;


class InputManager {
public:
    explicit InputManager(EventBus& bus);

    const Keyboard& Keys() const    { return keyboard; }
    const Mouse& Cursor() const     { return mouse; }
    const Touch& Touches() const    { return touch; }

   
    void EndFrame();

private:
    Keyboard keyboard;
    Mouse mouse;
    Touch touch;
};