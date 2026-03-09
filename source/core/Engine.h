#pragma once

#include <chrono>
#include <thread>
#include <iostream>

class EventBus;
class Scene;
class Renderer;
class PhysicsEngine;
class UiInput;
class UiRender;
class MouseInput;
class KeyboardInput;
class GLFWwindow;
class CameraSystem;
class ShadowSystem;
struct RenderContext;

class Engine {
public:
    Engine();
    ~Engine();

    void run();
    void stop();
    long long getTimeMicro();

private:
    EventBus* bus;
    Scene* scene;
    Renderer* renderer;
    PhysicsEngine* physicsEngine;
    UiInput* ui;
    UiRender* uiRender;
    MouseInput* mouseInput;
    KeyboardInput* keyboardInput;
    CameraSystem* cameraSystem;
	ShadowSystem* shadowSystem;
	RenderContext* renderContext;
    GLFWwindow* window;

    bool running;
    float accumulator;
    const float PHYSICS_STEP = 1.0f / 60.0f;

    int framecount;
    int framesThisSecond;
    float timeSinceLastFpsPrint;

    
};
