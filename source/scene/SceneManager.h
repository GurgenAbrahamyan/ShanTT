#pragma once

#include <vector>
#include <memory>
#include "Scene.h"


class SceneManager
{
public:

    explicit SceneManager(SceneContext& context) : context(context) {}


    template<typename T, typename... Args>
    T& PushScene(Args&&... args);


    void PopScene();


    template<typename T, typename... Args>
    T& ReplaceScene(Args&&... args);


    void Update(float dt);

    void FixedUpdate(float dt);


    Scene* Current();


    bool Empty() const
    {
        return scenes.empty();
    }


private:

    SceneContext& context;

    std::vector<std::unique_ptr<Scene>> scenes;
};

template<typename T, typename... Args>
T& SceneManager::PushScene(Args&&... args)
{
    static_assert(
        std::is_base_of_v<Scene, T>,
        "T must derive from Scene"
    );


    auto scene = std::make_unique<T>(
        std::forward<Args>(args)...
    );


    T& reference = *scene;


    if (Current())
        Current()->Pause();

    reference.Initialize(context);
    reference.Enter();

    scenes.push_back(std::move(scene));


    return reference;
}


template<typename T, typename... Args>
T& SceneManager::ReplaceScene(Args&&... args)
{
    while (!scenes.empty())
    {
        PopScene();
    }


    return PushScene<T>(
        std::forward<Args>(args)...
    );
}