#include "Scene.h"
#include "../core/EventBus.h"
#include "../core/Event.h"

#include "../resources/managers/TextureManager.h"
#include "../resources/managers/MaterialManager.h"
#include "../resources/managers/MeshManager.h"
#include "../resources/managers/ModelManager.h"

#include "../ecs/factories/GraphicsEntityFactory.h"
#include "../ecs/factories/PhysicsComponentFactory.h"

#include "../ecs/components/graphics/LightComponent.h"
#include "../ecs/components/physics/RigidBodyComponent.h"
#include "../ecs/components/physics/CollisionShapeComponent.h"
#include "../ecs/components/graphics/CameraComponent.h"
#include "../ecs/components/graphics/ModelComponent.h"
#include "../ecs/components/core/TagComponent.h"
#include "../core/EngineContext.h"
#include "../resources/data/ModelLoadConfig.h" 
Scene::Scene(EventBus* bus) {
    textureManager = new TextureManager();
    materialManager = new MaterialManager(textureManager, bus);
    meshManager = new MeshManager(bus);
    modelManager = new ModelManager(bus, meshManager, materialManager, textureManager);

   /* bus->subscribe<CreateObject>([this](CreateObject& event) {
        static int count = 0;

        std::string modelPath = "resource/models/duck/Duck.gltf";

        auto duck = modelManager->loadModel("duck", "resource/models/duck/Duck.gltf", registry, { Vector3(15,5,5), Quat(), Vector3(1,1,1) });

        for (int i = 0; i < 1000; i++) {
            float x = (rand() % 100) - 50;
            float z = (rand() % 100) - 50;

            auto entt = registry.create();


            registry.emplace<ModelComponent>(entt, registry.get<ModelComponent>(duck));
            registry.emplace<TransformComponent>(entt, Vector3(x, 0, z), Quat(), Vector3(1, 1, 1));
            registry.emplace<RigidBodyComponent>(entt, PhysicsComponentFactory::createRigidBody(registry, entt, Vector3(x, 0, z), Quat(), Vector3(), 1.0f));
            count++;
            std::cout << "Total Count:" << count;
        }
        });*/
}

Scene::~Scene() {

    delete modelManager;
    delete meshManager;
    delete materialManager;
    delete textureManager;
}

void Scene::initObjects() {

    textureManager->initDefaults();
    std::vector<std::string> faces = {
        "resource/textures/skybox/right.jpg",
        "resource/textures/skybox/left.jpg",
        "resource/textures/skybox/top.jpg",
        "resource/textures/skybox/bottom.jpg",
        "resource/textures/skybox/front.jpg",
        "resource/textures/skybox/back.jpg",
    };
    //skybox = textureManager->loadCubeMapArray(faces);
    skybox = textureManager->loadCubeMapHDR("resource\\textures\\hdr\\cedar_bridge_sunset_1_4k.hdr");
   // skybox = textureManager->loadCubeMapHDR("resource\\textures\\hdr\\warm_restaurant_night_4k.hdr");
    GraphicsEntityFactory::createSkybox(registry, skybox);

    CameraComponent cameraComp;

    GraphicsEntityFactory::createCamera(registry, Vector3(0, 7.5f, 4), cameraComp, true, "camera");


    // ??? Load assets (once) ?????????????????????????????
    modelManager->loadModel("chess", "resource/models/chess/chess_set_4k.gltf");
    modelManager->loadModel("map", "resource/models/map/scene.gltf");
    modelManager->loadModel("duck", "resource/models/duck/Duck.gltf");
    modelManager->loadModel("helmet", "resource/models/helmet/DamagedHelmet.gltf");
    modelManager->loadModel("car", "resource/models/toy car/ToyCar.gltf");
    modelManager->loadModel("boombox", "resource/models/boombox_4k/boombox_4k.gltf");
    modelManager->loadModel("cannon", "resource/models/cannon_4k.gltf/cannon_01_4k.gltf");


    // ??? Create entities + assign models ?????????????????
    auto chess = registry.create();
	registry.emplace<TagComponent>(chess, "Chess Set");
    auto map = registry.create();
	registry.emplace<TagComponent>(map, "Map");
    auto duck = registry.create();
	registry.emplace<TagComponent>(duck, "Duck");
    auto helmet = registry.create();
	registry.emplace<TagComponent>(helmet, "Helmet");
    auto car = registry.create();
	registry.emplace<TagComponent>(car, "Car");
    auto boombox = registry.create();
	registry.emplace<TagComponent>(boombox, "Boombox");
    auto cannon = registry.create();
	registry.emplace<TagComponent>(cannon, "Cannon");

    modelManager->instantiateModel("chess", registry, chess);
    modelManager->instantiateModel("map", registry, map);
    modelManager->instantiateModel("duck", registry, duck);
    modelManager->instantiateModel("helmet", registry, helmet);
    modelManager->instantiateModel("car", registry, car);
    modelManager->instantiateModel("boombox", registry, boombox);
    modelManager->instantiateModel("cannon", registry, cannon);

    auto createRBWithModelTransform = [&](entt::entity e, float mass, const Vector3& pos, const Quat& rot, const Vector3& scale) {
        registry.emplace<RigidBodyComponent>(e, PhysicsComponentFactory::createRigidBody(registry, e, pos, rot, scale, mass));
        };


    createRBWithModelTransform(chess, 1.0f, Vector3(-10, -10, 3), Quat(), Vector3(6, 6, 6));

    // Map (static)
    createRBWithModelTransform(map, 1.0f, Vector3(0, -10, 3), Quat(), Vector3(0.2f, 0.2f, 0.2f));

    // Duck
    createRBWithModelTransform(duck, 1.0f, Vector3(10, -10, 3), Quat(), Vector3(1, 1, 1));
    
    // Helmet
    createRBWithModelTransform(helmet, 1.0f, Vector3(-10, 0, 3), Quat(), Vector3(1, 1, 1));

    // Car
    createRBWithModelTransform(car, 1.0f, Vector3(0, 0, 3), Quat(), Vector3(100, 100, 100));

    // Boombox
    createRBWithModelTransform(boombox, 1.0f, Vector3(10, 0, 3), Quat(), Vector3(3, 3, 3));

    // Cannon
    createRBWithModelTransform(cannon, 5.0f, Vector3(-10, 10, 3), Quat(), Vector3(3, 3, 3));


    registry.emplace<CollisionShapeComponent>(chess, PhysicsComponentFactory::createCubeShape(Vector3(3, 3, 3)));
    registry.emplace<CollisionShapeComponent>(map, PhysicsComponentFactory::createCubeShape(Vector3(0.2, 0.2, 0.2)));
    registry.emplace<CollisionShapeComponent>(duck, PhysicsComponentFactory::createCubeShape(Vector3(1, 1, 1)));
    registry.emplace<CollisionShapeComponent>(helmet, PhysicsComponentFactory::createCubeShape(Vector3(1, 1, 1)));
    registry.emplace<CollisionShapeComponent>(car, PhysicsComponentFactory::createCubeShape(Vector3(100, 100, 100)));
    registry.emplace<CollisionShapeComponent>(boombox, PhysicsComponentFactory::createCubeShape(Vector3(3, 3, 3)));
    registry.emplace<CollisionShapeComponent>(cannon, PhysicsComponentFactory::createCubeShape(Vector3(3, 3, 3)));
    
    auto rect5 = GraphicsEntityFactory::createRectangle(registry, *meshManager, *materialManager, "rect5", Vector3(0, 0, 0), Vector3(30, 30, 2));

    
    registry.emplace<RigidBodyComponent>(rect5, PhysicsComponentFactory::createStaticBody(registry, rect5));


    registry.emplace<CollisionShapeComponent>(rect5, PhysicsComponentFactory::createCubeShape(Vector3(1, 1, 1)));
    

    LightComponent dir2;
    dir2.type = LightType::Spot;
    dir2.color = Vector3(1, 1, 1);
    dir2.intensity = 8.0f;
    dir2.direction = Vector3(0, 0, -1);
    dir2.innerConeAngle = 0.85;
    dir2.outerConeAngle = 0.90f;
    dir2.castsShadow = true;
    GraphicsEntityFactory::createLight(registry, Vector3(-10, -10, 6), dir2, "SpotLight1");

 
    dir2.type = LightType::Spot;
    dir2.color = Vector3(1, 1, 1);
    dir2.intensity = 8.0f;
    dir2.direction = Vector3(0, 0, -1);
    dir2.innerConeAngle = 0.85;
    dir2.outerConeAngle = 0.90f;
    dir2.castsShadow = true;
    GraphicsEntityFactory::createLight(registry, Vector3(0, -10, 6), dir2, "SpotLight2");

    dir2.type = LightType::Spot;
    dir2.color = Vector3(1, 1, 1);
    dir2.intensity = 8.0f;
    dir2.direction = Vector3(0, 0, -1);
    dir2.innerConeAngle = 0.85;
    dir2.outerConeAngle = 0.90f;
    dir2.castsShadow = true;
    GraphicsEntityFactory::createLight(registry, Vector3(10, -10, 6), dir2, "SpotLight3");

    dir2.type = LightType::Spot;
    dir2.color = Vector3(1, 1, 1);
    dir2.intensity = 8.0f;
    dir2.direction = Vector3(0, 0, -1);
    dir2.innerConeAngle = 0.85;
    dir2.outerConeAngle = 0.90f;
    dir2.castsShadow = true;
    GraphicsEntityFactory::createLight(registry, Vector3(-10, 0, 6), dir2, "SpotLight4");

    
    dir2.type = LightType::Spot;
    dir2.color = Vector3(1, 1, 1);
    dir2.intensity = 8.0f;
    dir2.direction = Vector3(0, 0, -1);
    dir2.innerConeAngle = 0.85;
    dir2.outerConeAngle = 0.90f;
    dir2.castsShadow = true;
    GraphicsEntityFactory::createLight(registry, Vector3(0, 0, 6), dir2, "SpotLight5");


    dir2.type = LightType::Spot;
    dir2.color = Vector3(1, 1, 1);
    dir2.intensity = 8.0f;
    dir2.direction = Vector3(0, 0, -1);
    dir2.innerConeAngle = 0.85;
    dir2.outerConeAngle = 0.90f;
    dir2.castsShadow = true;
    GraphicsEntityFactory::createLight(registry, Vector3(10, 0, 6), dir2, "SpotLight6");

 
    dir2.type = LightType::Spot;
    dir2.color = Vector3(1, 1, 1);
    dir2.intensity = 8.0f;
    dir2.direction = Vector3(0, 0, -1);
    dir2.innerConeAngle = 0.85;
    dir2.outerConeAngle = 0.90f;
    dir2.castsShadow = true;
    GraphicsEntityFactory::createLight(registry, Vector3(-10, 10, 6), dir2, "SpotLight7");
   

     




      /* dir2;
      dir2.type = LightType::Point;
      dir2.color = Vector3(1, 0, 0);
      dir2.intensity = 3.0f;
      dir2.direction = Vector3(0, 0, -1);
      dir2.innerConeAngle = 0.85;
      dir2.outerConeAngle = 0.90f;
      dir2.castsShadow = true;
      GraphicsEntityFactory::createLight(registry, Vector3(5, 5, 6), dir2, "PointLight1");*/
      // Spot light at (5,5,6)
    /*  LightComponent spot1;
      spot1.type = LightType::Spot;
      spot1.color = Vector3(0, 0, 1);
      spot1.intensity = 1.0f;
      spot1.direction = Vector3(0,0,-1);
      spot1.innerConeAngle = 0.70;
      spot1.outerConeAngle = 0.75f;

      GraphicsEntityFactory::createLight(registry, Vector3(5, 5, 6), spot1, "SpotLight3");

      // Point light at (10,8,5)
      LightComponent point1;
      point1.type = LightType::Point;
      point1.color = Vector3(1, 0.2f, 1);
      point1.intensity = 1.0f;

      GraphicsEntityFactory::createLight(registry, Vector3(10, 8, 5), point1, "PointLight2");

      // Spot light at (20,5,8)
      LightComponent spot2;
      spot2.type = LightType::Spot;
      spot2.color = Vector3(0.2f, 1, 1);
      spot2.intensity = 1.0f;
      spot2.direction = Vector3(0, 0, -1);

      GraphicsEntityFactory::createLight(registry, Vector3(20, 5, 8), spot2, "SpotLight4");

      // Point light at (30,8,5)
      LightComponent point2;
      point2.type = LightType::Point;
      point2.color = Vector3(1, 0.2f, 1);
      point2.intensity = 1.0f;

      GraphicsEntityFactory::createLight(registry, Vector3(30, 8, 5), point2, "PointLight3");

      // Point light at (35,8,5)
      LightComponent point3;
      point3.type = LightType::Point;
      point3.color = Vector3(0.2f, 1, 1);
      point3.intensity = 1.0f;

      GraphicsEntityFactory::createLight(registry, Vector3(35, 8, 5), point3, "PointLight4");*/
}

Texture* Scene::getBRDF(){
   
    return textureManager->getBRDF();
}
ModelManager* Scene::getModelManager() const{
    return modelManager;
}