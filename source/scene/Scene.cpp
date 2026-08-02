// Scene.cpp

#include "Scene.h"
#include "SceneContext.h"
#include "../core/ecs_systems/ISystem.h"


void Scene::Initialize(SceneContext& ctx)
{
    context = &ctx;

    OnCreate();

    for(auto& system : systems)
    {
        system->Initialize(ctx);
    }
}


void Scene::Update(float dt)
{
    for(auto& system : systems)
    {
        system->Update(*context, dt);
    }
}


void Scene::FixedUpdate(float dt)
{
    for(auto& system : systems)
    {
        system->FixedUpdate(*context, dt);
    }
}


void Scene::Shutdown()
{
    for(auto& system : systems)
    {
        system->Shutdown(*context);
    }

    OnDestroy();
}

