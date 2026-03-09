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

#include "../core/EngineContext.h"
#include "../resources/data/ModelLoadConfig.h" 
Scene::Scene(EventBus* bus) {
    textureManager = new TextureManager();
    materialManager = new MaterialManager(textureManager, bus);
    meshManager = new MeshManager(bus);
    modelManager = new ModelManager(bus, meshManager, materialManager, textureManager);

    bus->subscribe<CreateObject>([this](CreateObject& event) {
        static int count = 0;
        for (int i = 0; i < 1000; i++) {
            float x = (rand() % 100) - 50;
            float z = (rand() % 100) - 50;

            
            std::string modelPath = "resource/models/duck/Duck.gltf";

            auto duck = modelManager->loadModel("duck", "resource/models/duck/Duck.gltf", registry, { Vector3(15,5,5), Quat(), Vector3(1,1,1) });
            registry.emplace<RigidBodyComponent>(duck, PhysicsComponentFactory::createRigidBody(registry, duck, Vector3(x,0,z), Quat(), Vector3(), 1.0f));
            count++;
            std::cout << "Total Count:" <<count ;
        }
        });
}

Scene::~Scene() {
    // registry destructor cleans up all entities
    // managers own their GPU resources
    delete modelManager;
    delete meshManager;
    delete materialManager;
    delete textureManager;
}

void Scene::initObjects() {
    std::vector<std::string> faces = {
        "resource/textures/skybox/right.jpg",
        "resource/textures/skybox/left.jpg",
        "resource/textures/skybox/top.jpg", 
        "resource/textures/skybox/bottom.jpg",
        "resource/textures/skybox/front.jpg",
        "resource/textures/skybox/back.jpg",
    };
    skybox = textureManager->loadCubeMapArray(faces);
    GraphicsEntityFactory::createSkybox(registry, skybox);

	CameraComponent cameraComp;

    GraphicsEntityFactory::createCamera(registry, Vector3(0, 10, 0), cameraComp, true, "camera");
        

    auto chess = modelManager->loadModel("chess", "resource/models/chess/chess_set_4k.gltf", registry, { Vector3(5,5,5), Quat(), Vector3(3, 3, 3) });
    auto map = modelManager->loadModel("map", "resource/models/map/scene.gltf", registry, { Vector3(10,5,5), Quat(), Vector3(0.2,0.2,0.2) });
    auto duck = modelManager->loadModel("duck", "resource/models/duck/Duck.gltf", registry, { Vector3(15,5,5), Quat(), Vector3(1,1,1) });
    auto helmet = modelManager->loadModel("helmet", "resource/models/helmet/DamagedHelmet.gltf", registry, { Vector3(20,5,5), Quat(), Vector3(1,1,1) });
   // auto car = modelManager->loadModel("car", "resource/models/toy car/ToyCar.gltf", registry, { Vector3(25,5,5), Quat(), Vector3(100,100,100) });
    auto boombox = modelManager->loadModel("boombox", "resource/models/boombox_4k/boombox_4k.gltf", registry, { Vector3(30,5,5), Quat(), Vector3(3,3,3) });
    auto cannon = modelManager->loadModel("cannon", "resource/models/cannon_4k.gltf/cannon_01_4k.gltf", registry, { Vector3(35,5,5), Quat(), Vector3(3,3,3) });

    auto createRBWithModelTransform = [&](entt::entity e, float mass, const Vector3& pos, const Quat& rot, const Vector3& scale) {
        registry.emplace<RigidBodyComponent>(e, PhysicsComponentFactory::createRigidBody(registry, e, pos, rot, scale, mass));
        };

   
    createRBWithModelTransform(chess, 1.0f, Vector3(5, 5, 5), Quat(), Vector3(6, 6, 6));

    // Map (static)
    createRBWithModelTransform(map, 1.0f, Vector3(10, 5, 5), Quat(), Vector3(0.2f, 0.2f, 0.2f));

    // Duck
    createRBWithModelTransform(duck, 1.0f, Vector3(15, 5, 5), Quat(), Vector3(1, 1, 1));

    // Helmet
    createRBWithModelTransform(helmet, 1.0f, Vector3(20, 5, 5), Quat(), Vector3(1, 1, 1));

    // Car
  //  createRBWithModelTransform(car, 1.0f, Vector3(25, 5, 5), Quat(), Vector3(100, 100, 100));

    // Boombox
    createRBWithModelTransform(boombox, 1.0f, Vector3(30, 5, 5), Quat(), Vector3(3, 3, 3));

    // Cannon
    createRBWithModelTransform(cannon, 5.0f, Vector3(35, 5, 5), Quat(), Vector3(3, 3, 3));


    registry.emplace<CollisionShapeComponent>(chess, PhysicsComponentFactory::createCubeShape(Vector3(3, 3, 3)));
    registry.emplace<CollisionShapeComponent>(map, PhysicsComponentFactory::createCubeShape(Vector3(0.2, 0.2, 0.2)));
    registry.emplace<CollisionShapeComponent>(duck, PhysicsComponentFactory::createCubeShape(Vector3(1, 1, 1)));
    registry.emplace<CollisionShapeComponent>(helmet, PhysicsComponentFactory::createCubeShape(Vector3(1, 1, 1)));
  //  registry.emplace<CollisionShapeComponent>(car, PhysicsComponentFactory::createCubeShape(Vector3(100, 100, 100)));
    registry.emplace<CollisionShapeComponent>(boombox, PhysicsComponentFactory::createCubeShape(Vector3(3, 3, 3)));
    registry.emplace<CollisionShapeComponent>(cannon, PhysicsComponentFactory::createCubeShape(Vector3(3, 3, 3)));

    
    auto rect1 = GraphicsEntityFactory::createRectangle(registry, *meshManager,*materialManager,  "rect1", Vector3(5, 0, 0), Vector3(3, 3, 3));
    auto rect2 = GraphicsEntityFactory::createRectangle(registry, *meshManager, *materialManager, "rect2", Vector3(0, 5, 0), Vector3(2, 2, 2));
    auto rect3 = GraphicsEntityFactory::createRectangle(registry, *meshManager, *materialManager, "rect3", Vector3(0, 0, 5), Vector3(1, 1, 1));
    auto rect4 = GraphicsEntityFactory::createRectangle(registry, *meshManager, *materialManager, "rect4", Vector3(10, 0, 0), Vector3(1, 1, 1));
    auto rect5 = GraphicsEntityFactory::createRectangle(registry, *meshManager, *materialManager, "rect5", Vector3(0, 0, 0), Vector3(500, 500, 5));

    registry.emplace<RigidBodyComponent>(rect1, PhysicsComponentFactory::createStaticBody(registry, rect1));
    registry.emplace<RigidBodyComponent>(rect2, PhysicsComponentFactory::createStaticBody(registry, rect2));
    registry.emplace<RigidBodyComponent>(rect3, PhysicsComponentFactory::createStaticBody(registry, rect3));
    registry.emplace<RigidBodyComponent>(rect4, PhysicsComponentFactory::createStaticBody(registry, rect4));
    registry.emplace<RigidBodyComponent>(rect5, PhysicsComponentFactory::createStaticBody(registry, rect5));


    registry.emplace<CollisionShapeComponent>(rect1, PhysicsComponentFactory::createCubeShape(Vector3(3, 3, 3)));
    registry.emplace<CollisionShapeComponent>(rect2, PhysicsComponentFactory::createCubeShape(Vector3(2, 2, 2)));
    registry.emplace<CollisionShapeComponent>(rect3, PhysicsComponentFactory::createCubeShape(Vector3(1, 1, 1)));
    registry.emplace<CollisionShapeComponent>(rect4, PhysicsComponentFactory::createCubeShape(Vector3(1, 1, 1)));
    registry.emplace<CollisionShapeComponent>(rect5, PhysicsComponentFactory::createCubeShape(Vector3(1, 1, 1)));


    LightComponent dir2;
    dir2.type = LightType::Spot;
    dir2.color = Vector3(1, 1, 1);
    dir2.intensity = 1.0f;
    dir2.direction = Vector3(0, 0, -1);
    dir2.innerConeAngle = 0.85;
    dir2.outerConeAngle = 0.90f;
    dir2.castsShadow = true;
    GraphicsEntityFactory::createLight(registry, Vector3(20, 5, 8), dir2, "DirLight2");


    dir2;
    dir2.type = LightType::Spot;
    dir2.color = Vector3(1, 1, 1);
    dir2.intensity = 1.0f;
    dir2.direction = Vector3(0, -1, -1);
    dir2.innerConeAngle = 0.85;
    dir2.outerConeAngle = 0.90f;
    dir2.castsShadow = true;
    GraphicsEntityFactory::createLight(registry, Vector3(20, 8, 8), dir2, "DirLight2");


   /* LightComponent dir1;
    dir1.type = LightType::Directional;
    dir1.color = Vector3(1, 1, 1);
    dir1.intensity = 1.0f;
    dir1.direction = Vector3(0, 0, -1);
    dir1.innerConeAngle = 0.70;
    dir1.outerConeAngle = 0.75f;
	dir1.castsShadow = true;
    GraphicsEntityFactory::createLight(registry, Vector3(15, 0, 10), dir1, "DirLight1");
    
    */



    
   /*  dir2;
    dir2.type = LightType::Point;
    dir2.color = Vector3(1, 0, 0);
    dir2.intensity = 0.1f;
    dir2.direction = Vector3(0, 0, -1);
    dir2.innerConeAngle = 0.85;
    dir2.outerConeAngle = 0.90f;
  //  dir2.castsShadow = true;
    GraphicsEntityFactory::createLight(registry, Vector3(20, 5, 10), dir2, "DirLight2");*/
    // Spot light at (5,5,6)
   /* LightComponent spot1;
    spot1.type = LightType::Spot;
    spot1.color = Vector3(0, 0, 1);
    spot1.intensity = 1.0f;
    spot1.direction = Vector3(0,0,-1);
    spot1.innerConeAngle = 0.70;
    spot1.outerConeAngle = 0.75f;

    GraphicsEntityFactory::createLight(registry, Vector3(5, 5, 6), spot1, "SpotLight1");

    // Point light at (10,8,5)
    LightComponent point1;
    point1.type = LightType::Point;
    point1.color = Vector3(1, 0.2f, 1);
    point1.intensity = 1.0f;

    GraphicsEntityFactory::createLight(registry, Vector3(10, 8, 5), point1, "PointLight1");

    // Spot light at (20,5,8)
    LightComponent spot2;
    spot2.type = LightType::Spot;
    spot2.color = Vector3(0.2f, 1, 1);
    spot2.intensity = 1.0f;
    spot2.direction = Vector3(0, 0, -1);

    GraphicsEntityFactory::createLight(registry, Vector3(20, 5, 8), spot2, "SpotLight2");

    // Point light at (30,8,5)
    LightComponent point2;
    point2.type = LightType::Point;
    point2.color = Vector3(1, 0.2f, 1);
    point2.intensity = 1.0f;

    GraphicsEntityFactory::createLight(registry, Vector3(30, 8, 5), point2, "PointLight2");

    // Point light at (35,8,5)
    LightComponent point3;
    point3.type = LightType::Point;
    point3.color = Vector3(0.2f, 1, 1);
    point3.intensity = 1.0f;

    GraphicsEntityFactory::createLight(registry, Vector3(35, 8, 5), point3, "PointLight3");*/
}