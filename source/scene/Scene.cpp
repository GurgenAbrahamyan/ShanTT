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

    for(auto& extractionSystem : extractionSystems)
        extractionSystem->onInit(registry);

    state = SceneState::Exited;
}

void Scene::Shutdown()
{
    if(!context) return;


    for(auto it = systems.rbegin(); it != systems.rend(); ++it)
        (*it)->Shutdown(*context);
    

    OnDestroy();

    systems.clear();

    context = nullptr;

    state = SceneState::Destroyed;
}

void Scene::Enter()
{

    if(state != SceneState::Exited)
        return;

    OnEnter();

    state = SceneState::Active;
}

void Scene::Exit()
{
    if(state == SceneState::Exited) 
        return;

    OnExit();

    state = SceneState::Exited;
}

void Scene::Update(float dt)
{
    if (state != SceneState::Active)
         return;


    for(auto& system : systems)
    {
        system->Update(*context, dt);
    }
}

void Scene::Pause(){

    if(state != SceneState::Active) 
        return;

    OnPause();

    state = SceneState::Paused;

}

void Scene::Resume(){

    if(state != SceneState::Paused)
        return;

    OnResume();

    state = SceneState::Active;
}

void Scene::FixedUpdate(float dt)
{
    if (state != SceneState::Active)
         return;

    for(auto& system : systems)
    {
        system->FixedUpdate(*context, dt);
    }
}



