#pragma once
#include "../math_custom/Vector2.h"

class EventBus;

enum class GraphicsAPI { None, OpenGL, Vulkan };

struct WindowDesc {
    int width = 1280;
    int height = 720;
    const char* title = "Hexium";
    GraphicsAPI api = GraphicsAPI::OpenGL;
    bool vsync = true;
};

class IPlatform {
public:
    virtual ~IPlatform() = default;

    virtual bool Init(const WindowDesc& desc, EventBus& bus) = 0;
    virtual void Shutdown() = 0;

    virtual void PollEvents() = 0;      
    virtual void SwapBuffers() = 0;    
    virtual bool ShouldClose() const = 0;
    virtual void RequestClose() = 0;

    virtual void* GetNativeWindowHandle() const = 0;
    virtual Vector2 GetFramebufferSize() const = 0;
    virtual double GetTime() const = 0;
};

std::unique_ptr<IPlatform> CreatePlatform(); 