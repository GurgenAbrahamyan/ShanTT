#include "SceneManager.h"

void SceneManager::PopScene()
{
    if (scenes.empty())
        return;


    auto& scene = scenes.back();

    scene->Exit();
    scene->Shutdown();

    scenes.pop_back();


    if (!scenes.empty())
    {
        scenes.back()->Resume();
    }
}


void SceneManager::Update(float dt)
{
    if (scenes.empty())
        return;


    scenes.back()->Update(dt);
}


void SceneManager::FixedUpdate(float dt)
{
    if (scenes.empty())
        return;


    scenes.back()->FixedUpdate(dt);
}


Scene* SceneManager::Current()
{
    if (scenes.empty())
        return nullptr;


    return scenes.back().get();
}