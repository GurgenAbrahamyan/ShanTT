#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <vector>

#include "../core/ecs_systems/ISystem.h"
#include "SceneContext.h"

class Scene
{
public:

    virtual ~Scene() = default;


    void Initialize(SceneContext& ctx);

    void Update(float dt);

    void FixedUpdate(float dt);

    void Shutdown();


    entt::registry& Registry()
    {
        return registry;
    }

    template <typename T>
    T* GetSystem();

protected:

    virtual void OnCreate() = 0;

    virtual void OnDestroy(){}


    template<typename T, typename... Args>
    T& AddSystem(Args&&... args);


    template<typename T>
    void RemoveSystem();


private:

    entt::registry registry;

    std::vector<std::unique_ptr<ISystem>> systems;

    SceneContext* context = nullptr;
};



template<typename T, typename... Args>
T& Scene::AddSystem(Args&&... args)
{
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