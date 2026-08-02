#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <vector>

#include "../core/ecs_systems/ISystem.h"
#include "SceneContext.h"

class Scene
{

public:

    enum class SceneState {
        Created,
        Active,
        Paused,
        Exited,
        Destroyed
    };

    virtual ~Scene() = default;


    void Initialize(SceneContext& ctx);
    void Shutdown();

    void Enter();
    void Exit();

    void Pause();
    void Resume();

    void Update(float dt);
    void FixedUpdate(float dt);

    


    entt::registry& Registry()
    {
        return registry;
    }

    template <typename T>
    T* GetSystem();


    SceneState GetState() const
    {
        return state;
    }

protected:

    virtual void OnCreate() {}

    virtual void OnDestroy(){}

    virtual void OnEnter()  {};

    virtual void OnExit()   {};

    virtual void OnPause()  {}

    virtual void OnResume() {}


    template<typename T, typename... Args>
    T& AddSystem(Args&&... args);


    template<typename T>
    void RemoveSystem();


private:
    SceneState state { SceneState::Created };

    entt::registry registry;

    std::vector<std::unique_ptr<ISystem>> systems;

    SceneContext* context = nullptr;
};



template<typename T, typename... Args>
T& Scene::AddSystem(Args&&... args)
{

    static_assert(
        std::is_base_of_v<ISystem, T>,
        "T must derive from ISystem"
    );


    auto system = std::make_unique<T>(
        std::forward<Args>(args)...
    );

    T& ref = *system;

    systems.push_back(
        std::move(system)
    );

    return ref;
}

template<typename T>
T* Scene::GetSystem()
{
    for (auto& system : systems) {
        if (auto* castedSystem = dynamic_cast<T*>(system.get())) {
            return castedSystem;
        }
    }
    return nullptr;
}