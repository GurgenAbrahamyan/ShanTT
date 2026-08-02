#include "Engine.h"

#include "platform/IPlatform.h"
#include "EventBus.h"
#include "../render/Renderer.h"
#include "../physics/PhysicsEngine.h"
//#include "../input/UIInput.h"
#include "../input/KeyBoardInput.h"
#include "ecs_systems/CameraSystem.h"
#include "../render/ecs_systems/ShadowSystem.h"
#include "../input/MouseInput.h"
#include "EngineContext.h"
#include <chrono>


Engine::Engine()
    : platform(CreatePlatform()),
      bus(new EventBus()),
      scene(/*new Scene(bus)*/),
      physicsEngine(new PhysicsEngine()),
      cameraSystem(/*new CameraSystem (bus, scene->getRegistry())*/),
      shadowSystem(new ShadowSystem()),
      running(true),
      accumulator(0.0f),
      framesThisSecond(0),
      timeSinceLastFpsPrint(0.0f)
{
    renderContext = new RenderContext(),
    renderer = new Renderer(bus, renderContext);

    
   
    keyboardInput = new KeyboardInput(bus);
    mouseInput = new MouseInput(bus); 
    /*
    scene->initObjects();
	renderContext->registry = &scene->getRegistry();
    renderContext->brdfTexture = scene->getBRDF();
    renderContext->modelManager = scene->getModelManager();*/
}

Engine::~Engine() {
    delete bus;
    delete scene;
    delete renderer;
    delete physicsEngine;
   
    delete keyboardInput;
    delete mouseInput;
}

void Engine::run() {
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {

        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> delta = now - lastTime;
        lastTime = now;
        float frameTime = delta.count();


        accumulator += frameTime;


        while (accumulator >= PHYSICS_STEP) {
            
           // physicsEngine->update(scene->getRegistry(), PHYSICS_STEP);
            accumulator -= PHYSICS_STEP;
        }

        EngineContext::get().deltaTime = frameTime;

        keyboardInput->processInput();
       // cameraSystem->update(scene->getRegistry(), frameTime);
        mouseInput->proccessInput(renderContext->windowWidth, renderContext->windowHeight);
        
        glfwPollEvents();          

       
        renderer->rebuildContext(renderContext);
        shadowSystem->update(renderContext);
        renderer->render();       

        

        glfwSwapBuffers(window);
     

        framesThisSecond++;
        timeSinceLastFpsPrint += frameTime;

        if (timeSinceLastFpsPrint >= 1.0f) {
            std::cout << "FPS: " << framesThisSecond << "\n";
            framesThisSecond = 0;
            timeSinceLastFpsPrint = 0.0f;

        }
    }
}

void Engine::stop() {
    running = false;
}

long long Engine::getTimeMicro() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}