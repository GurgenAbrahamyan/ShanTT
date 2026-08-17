#include "Mouse.h"

void Mouse::OnButtonEvent(const MouseButtonEvent& e)
{
    const size_t i = Index(e.button);
    const bool wasDown = current[i];
    const bool isDown  = (e.action == InputAction::Pressed);

    current[i] = isDown;
    if (isDown && !wasDown)  pressed[i]  = true;
    if (!isDown && wasDown)  released[i] = true;
}

void Mouse::OnMoveEvent(const MouseMoveEvent& e)
{
    pos = Vector2(static_cast<float>(e.x), static_cast<float>(e.y));
}

void Mouse::OnScrollEvent(const ScrollEvent& e)
{
    scroll  = Vector2(static_cast<float>(e.xoffset), static_cast<float>(e.yoffset));
}

void Mouse::EndFrame()
{
    pressed.fill(false);
    released.fill(false);
    lastPos = pos;
    scroll = Vector2();
}