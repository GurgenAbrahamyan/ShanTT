#pragma once
#include <GLFW/glfw3.h>

class EngineContext {
public:
    float deltaTime = 0.0f;
    float totalTime = 0.0f;
   

	
    static EngineContext& get() {
        static EngineContext instance;
        return instance;
    }

    void setWindow(GLFWwindow* win) { window = win; }
    GLFWwindow* getWindow() { return window; }

private:
    EngineContext() = default;
    GLFWwindow* window = nullptr;
};
