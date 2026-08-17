#pragma once

enum class GraphicsAPI { None, OpenGL, Vulkan };

struct WindowDesc {
    int width = 1920;
    int height = 1200;
    const char* title = "ShanTT";
    GraphicsAPI api = GraphicsAPI::OpenGL;
    bool vsync = true;
};
