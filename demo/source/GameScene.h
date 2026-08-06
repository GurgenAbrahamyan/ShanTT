#pragma once
#include "scene/Scene.h"
#include "resources/assets/CubeMap.h"

class GameScene : public Scene
{
public:
    Texture* getBRDF();

protected:
    void OnCreate() override;

private:
    CubeMap* skybox = nullptr;
};