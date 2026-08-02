#pragma once
class AssetManager;
class InputManager;
struct GLFWwindow;
class EventBus;
class Renderer;
class PhysicsEngine;

struct EngineContext
{
    GLFWwindow* window = nullptr;

    float deltaTime = 0.0f;

    float totalTime = 0.0f;


    EventBus& events;

    Renderer& renderer;

    PhysicsEngine& physics;

    AssetManager& assets;

    InputManager& input;
};