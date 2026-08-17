#include "core/Engine.h"
#include "source/GameScene.h"
int main(){

    Engine engine{};

    engine.getSceneContext().engine.sceneManager->PushScene<GameScene>();

    engine.run();
}