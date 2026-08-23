#include "Engine.h"

#include "../render/Renderer.h"
#include "../physics/PhysicsEngine.h"

#include <chrono>
#include <iostream>

Engine::Engine()
    : platform(CreatePlatform()),
      bus(),
      input(bus),
      debugUi(*platform.get(), &bus, assetManager), 
      renderer(),
      physicsEngine(PhysicsEngine()),
      engineContext({*platform.get(),
                    0.0f,
                    0.0f, 
                    bus,
                    renderer,
                    physicsEngine,
                    assetManager,
                    input,
                    nullptr}),
      sceneContext(engineContext),
      sceneManager(sceneContext),
      running(true),
      accumulator(0.0f),
      framesThisSecond(0),
      timeSinceLastFpsPrint(0.0f)

{
    WindowDesc desc;
    desc.width = 1920;
    desc.height = 1200;
    desc.title = "ShanTT";
    desc.api = GraphicsAPI::OpenGL;

    engineContext.sceneManager = &sceneManager;

    if (!platform->Init(desc, bus)) {
        throw std::runtime_error("Platform init failed");
    }

    renderer.Init();
    debugUi.Initialize();

}

Engine::~Engine() {
    platform->Shutdown();
}

void Engine::run() {
    while (!platform->ShouldClose() && running) {

        double frameStart = platform->GetTime();
        static double lastTime = frameStart;
        float frameTime = static_cast<float>(frameStart - lastTime);
        lastTime = frameStart;

        platform->PollEvents();

        accumulator += frameTime;
        while (accumulator >= PHYSICS_STEP) {
            physicsEngine.update(sceneManager.Current()->Registry(), PHYSICS_STEP);
            sceneManager.FixedUpdate(PHYSICS_STEP);
            accumulator -= PHYSICS_STEP;
        }

        sceneManager.Update(frameTime);
    
        for( auto& extract : sceneManager.Current()->GetExtractors())
            extract->extract(sceneManager.Current()->Registry(), frameData);

        renderer.render(frameData);

        debugUi.startNewFrame();
        debugUi.buildUI(
            sceneManager.Current()->Registry(),
            platform->GetFramebufferSize(),
            renderer.getDebugRenderData(),
            renderer.getRenderGraph()
        );
        debugUi.render();

        platform->SwapBuffers();

        input.EndFrame();

        framesThisSecond++;
        timeSinceLastFpsPrint += frameTime;
        if (timeSinceLastFpsPrint >= 1.0f) {
            std::cout << "FPS: " << framesThisSecond << "\n";
            framesThisSecond = 0;
            timeSinceLastFpsPrint = 0.0f;
        }

        frameData.Clear();
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