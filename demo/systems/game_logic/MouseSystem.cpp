// MouseInput.cpp
#include "MouseInput.h"
#include "SceneContext.h"
#include "core/Event.h"
#include "imgui/imgui.h"

void MouseInput::Update(SceneContext& ctx, float dt)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return; 

    auto& input = ctx.engine.input;
    auto& cursor = input.Cursor();

    if (cursor.WasPressed(MouseButton::Left) && !cameraMode)
    {
        cameraMode = true;
        ctx.engine.events.publish(CameraMode{ cameraMode });
        ctx.engine.platform.SetCursorMode(CursorMode::Disabled);
    }

    if (cursor.WasPressed(MouseButton::Right) && cameraMode)
    {
        cameraMode = false;
        ctx.engine.events.publish(CameraMode{ cameraMode });
        ctx.engine.platform.SetCursorMode(CursorMode::Normal);
    }

    if (cameraMode)
    {
        Vector2 delta = cursor.Delta();
        if (delta.x != 0.0f || delta.y != 0.0f)
            ctx.engine.events.publish(MouseDragged{ delta.x, delta.y });
    }
}