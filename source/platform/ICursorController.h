#pragma once
#include "../math_custom/Vector2.h"

enum class CursorMode { Normal, Hidden, Disabled };

class ICursorController {
public:
    virtual ~ICursorController() = default;

    virtual void SetCursorMode(CursorMode mode) = 0;
    virtual void SetCursorPosition(const Vector2& pos) = 0;
    virtual Vector2 GetCursorPosition() const = 0;
};