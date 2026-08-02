#pragma once
#include <memory>
class IPlatform;
class EventBus;
class Renderer;
class PhysicsEngine;
class UiInput;
class MouseInput;
class KeyboardInput;
struct GLFWwindow;
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
    std::unique_ptr<IPlatform> platform;
    std::unique_ptr<EventBus> bus;

    Renderer* renderer;
    PhysicsEngine* physicsEngine;
   
    MouseInput* mouseInput;
    KeyboardInput* keyboardInput;
    [[maybe_unused]] CameraSystem* cameraSystem;
	ShadowSystem* shadowSystem;
	RenderContext* renderContext;
    

    bool running;
    float accumulator;
    const float PHYSICS_STEP = 1.0f / 60.0f;

  
    int framesThisSecond;
    float timeSinceLastFpsPrint;

    
};
