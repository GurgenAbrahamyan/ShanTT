#pragma once
#include "scene/Scene.h"
#include "resources/managers/EnvironmentBaker.h"

class GameScene : public Scene
{
protected:
    void OnCreate() override;

private:
    EnvironmentBaker baker;

};