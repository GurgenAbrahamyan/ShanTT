#include "Keyboard.h"

void Keyboard::OnKeyEvent(const KeyEvent& e)
{
    const size_t i = Index(e.key);
    const bool wasDown = current[i];
    const bool isDown  = (e.action == InputAction::Pressed);

    current[i] = isDown;
    if (isDown && !wasDown)  pressed[i]  = true;
    if (!isDown && wasDown)  released[i] = true;
}

void Keyboard::EndFrame()
{
    pressed.fill(false);
    released.fill(false);
}