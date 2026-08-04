#pragma once
#include "../IPlatform.h"

struct GLFWwindow;
class EventBus;
class WindowGLFW;
class PlatformGLFW final : public IPlatform {
public:
    bool Init(const WindowDesc&, EventBus& bus) override;

    void Shutdown() override;

    void PollEvents() override;
    void SwapBuffers() override;
    bool ShouldClose() const override;
    void RequestClose() override;

    void* GetNativeWindowHandle() const override;
    Vector2 GetFramebufferSize() const override;
    double GetTime() const override;


private:
    static void KeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* w, int button, int action, int mods);
    static void CursorPosCallback(GLFWwindow* w, double x, double y);
    static void ScrollCallback(GLFWwindow* w, double xoff, double yoff);

    GLFWwindow* window = nullptr;
};