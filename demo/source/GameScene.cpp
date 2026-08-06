#include "GameScene.h"

#include "ecs/factories/GraphicsEntityFactory.h"
#include "ecs/factories/PhysicsComponentFactory.h"

#include "ecs/components/graphics/LightComponent.h"
#include "ecs/components/physics/RigidBodyComponent.h"
#include "ecs/components/physics/CollisionShapeComponent.h"
#include "ecs/components/graphics/CameraComponent.h"
#include "ecs/components/core/TagComponent.h"

void GameScene::OnCreate()
{
    auto& assets = Context().engine.assets;
    auto& registry = Registry();

    assets.textures().initDefaults();

    
    skybox = assets.textures().loadCubeMapHDR("resource\\textures\\hdr\\cedar_bridge_sunset_1_4k.hdr");

    GraphicsEntityFactory::createSkybox(registry, skybox);

    CameraComponent cameraComp;
    GraphicsEntityFactory::createCamera(registry, Vector3(0, 7.5f, 4), cameraComp, true, "camera");

    
    assets.models().loadModel("chess",   "resource/models/chess/chess_set_4k.gltf");
    assets.models().loadModel("map",     "resource/models/map/scene.gltf");
    assets.models().loadModel("duck",    "resource/models/duck/Duck.gltf");
    assets.models().loadModel("helmet",  "resource/models/helmet/DamagedHelmet.gltf");
    assets.models().loadModel("car",     "resource/models/toy car/ToyCar.gltf");
    assets.models().loadModel("boombox", "resource/models/boombox_4k/boombox_4k.gltf");
    assets.models().loadModel("cannon",  "resource/models/cannon_4k.gltf/cannon_01_4k.gltf");

    
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

    assets.models().instantiateModel("chess",   registry, chess);
    assets.models().instantiateModel("map",     registry, map);
    assets.models().instantiateModel("duck",    registry, duck);
    assets.models().instantiateModel("helmet",  registry, helmet);
    assets.models().instantiateModel("car",     registry, car);
    assets.models().instantiateModel("boombox", registry, boombox);
    assets.models().instantiateModel("cannon",  registry, cannon);

    auto createRBWithModelTransform = [&](entt::entity e, float mass, const Vector3& pos, const Quat& rot, const Vector3& scale) {
        registry.emplace<RigidBodyComponent>(e, PhysicsComponentFactory::createRigidBody(registry, e, pos, rot, scale, mass));
        };

    createRBWithModelTransform(chess,   1.0f, Vector3(-10, 3, -10), Quat(), Vector3(6, 6, 6));
    createRBWithModelTransform(map,     1.0f, Vector3(0, 3, -10),   Quat(), Vector3(0.2f, 0.2f, 0.2f));
    createRBWithModelTransform(duck,    1.0f, Vector3(10, 3, -10),  Quat(), Vector3(1, 1, 1));
    createRBWithModelTransform(helmet,  1.0f, Vector3(-10, 3, 0),   Quat(), Vector3(1, 1, 1));
    createRBWithModelTransform(car,     1.0f, Vector3(0, 3, 0),     Quat(), Vector3(100, 100, 100));
    createRBWithModelTransform(boombox, 1.0f, Vector3(10, 3, 0),    Quat(), Vector3(3, 3, 3));
    createRBWithModelTransform(cannon,  5.0f, Vector3(-10, 3, 10),  Quat(), Vector3(3, 3, 3));

    registry.emplace<CollisionShapeComponent>(chess,   PhysicsComponentFactory::createCubeShape(Vector3(3, 3, 3)));
    registry.emplace<CollisionShapeComponent>(map,     PhysicsComponentFactory::createCubeShape(Vector3(0.2f, 0.2f, 0.2f)));
    registry.emplace<CollisionShapeComponent>(duck,    PhysicsComponentFactory::createCubeShape(Vector3(1, 1, 1)));
    registry.emplace<CollisionShapeComponent>(helmet,  PhysicsComponentFactory::createCubeShape(Vector3(1, 1, 1)));
    registry.emplace<CollisionShapeComponent>(car,     PhysicsComponentFactory::createCubeShape(Vector3(100, 100, 100)));
    registry.emplace<CollisionShapeComponent>(boombox, PhysicsComponentFactory::createCubeShape(Vector3(3, 3, 3)));
    registry.emplace<CollisionShapeComponent>(cannon,  PhysicsComponentFactory::createCubeShape(Vector3(3, 3, 3)));

    auto rect5 = GraphicsEntityFactory::createRectangle(
        registry, assets.meshes(), assets.materials(), "rect5", Vector3(0, 0, 0), Vector3(30, 2, 30));

    registry.emplace<RigidBodyComponent>(rect5, PhysicsComponentFactory::createStaticBody(registry, rect5));
    registry.emplace<CollisionShapeComponent>(rect5, PhysicsComponentFactory::createCubeShape(Vector3(1, 1, 1)));

    LightComponent dir2;
    dir2.type = LightType::Directional;
    dir2.color = Vector3(1, 1, 1);
    dir2.intensity = 8.0f;
    dir2.innerConeAngle = 0.85f;
    dir2.outerConeAngle = 0.90f;
    dir2.castsShadow = true;
    GraphicsEntityFactory::createLight(
        registry, Vector3(0, 5, 0),
        Quat::fromAxisAngleDeg(Vector3(1.0f, 0.0f, 0.0f), -90.0f),
        dir2, "DirLight");
}

Texture* GameScene::getBRDF()
{
    return Context().engine.assets.textures().getBRDF();
}