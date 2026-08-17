#pragma once
#include <entt/entt.hpp>
struct SceneContext;

class ISystem
{
public:
    explicit ISystem(entt::registry& registry)
        : registry(registry)
    {}

    virtual ~ISystem() = default;

    virtual void Initialize(SceneContext&) {}
    virtual void Update(SceneContext&, float) {}
    virtual void FixedUpdate(SceneContext&, float) {}
    virtual void Shutdown(SceneContext&) {}

protected:
    entt::registry& registry;
};