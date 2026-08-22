#include "GameScene.h"

#include "ecs/factories/GraphicsEntityFactory.h"
#include "ecs/factories/PhysicsComponentFactory.h"

#include "ecs/components/graphics/LightComponent.h"
#include "ecs/components/physics/RigidBodyComponent.h"
#include "ecs/components/physics/CollisionShapeComponent.h"
#include "ecs/components/graphics/CameraComponent.h"
#include "ecs/components/core/TagComponent.h"
#include "ecs/components/graphics/CubeMapComponent.h"

#include "scene/GlobalTexturesExtractor.h"
#include "scene/EnvironmentExtractor.h"
#include "scene/SceneExtractor.h"

#include "render/handlers/ShadowPass.h"
#include "render/handlers/GeometryPass.h"
#include "render/handlers/LightingPass.h"
#include "render/handlers/CubeMapPass.h"
#include "render/handlers/BlurPass.h"
#include "render/handlers/BloomPass.h"
#include "render/handlers/CompositePass.h"
#include "render/handlers/ToneMappingPass.h"
#include "render/handlers/FXAAPass.h"
#include "render/handlers/FinalBlitPass.h"

#include "../systems/game_logic/CameraSystem.h"
#include "render/ecs_systems/ShadowSystem.h"

#include "ecs/factories/ModelSpawner.h"
void GameScene::OnCreate()
{
    auto& renderGraph = *Context().engine.renderer.getRenderGraph();
    auto& shaderManager = Context().engine.assets.shaders();

    uint32_t screenWidth { static_cast<uint32_t>(Context().engine.platform.GetFramebufferSize().x)}, 
             screenHeight{ static_cast<uint32_t>(Context().engine.platform.GetFramebufferSize().y)};
    shaderManager.load(
        "default_shadow",
        "source\\Shaders\\shadow_shader\\shadow_pass.vert",
        "source\\Shaders\\shadow_shader\\shadow_pass.frag",
        
        ShaderType::SHADOWMAP
    );

    auto* shadow = renderGraph.addPass<ShadowPass>(ShadowPass::ShadowPassOptions{ 
                                    shaderManager.getShader("default_shadow")});


    shaderManager.load(
        "default_obj",
        "source\\shaders\\object_shader\\default.vert",
        "source\\Shaders\\object_shader\\default.frag",
        
        ShaderType::OBJECT3D
    );

    shaderManager.load(
        "default_obj_skinned",
        "source\\shaders\\object_shader\\skinned.vert",
        "source\\Shaders\\object_shader\\default.frag",
        
        ShaderType::OBJECT3D
    );


    auto* geometry = renderGraph.addPass<GeometryPass>(GeometryPass::GeometryPassOptions{ 
                                    shaderManager.getShader("default_obj")
                                    , shaderManager.getShader("default_obj_skinned")
                                    , screenWidth
                                    , screenHeight});


    shaderManager.load(
        "default_light",
        "source\\shaders\\lighting_shader\\lighting_pass.vert",
        "source\\shaders\\lighting_shader\\lighting_pass.frag",
       
        ShaderType::LIGHT
    );

    auto* lighting = renderGraph.addPass<LightingPass>(LightingPass::LightingPassOptions{ 
                                    shaderManager.getShader("default_light")
                                    , geometry->albedo()
                                    , geometry->linearDepth()
                                    , geometry->normal()
                                    , geometry->arm()
                                    , geometry->emissive()
                                    , shadow->shadowTexture()
                                    , screenWidth
                                    , screenHeight});



    Shader* objShader = shaderManager.getShader("default_light");
    objShader->Activate();
    GLuint blockIndex = glGetUniformBlockIndex(objShader->ID, "LightBlock");
    glUniformBlockBinding(objShader->ID, blockIndex, 1);



   shaderManager.load(
        "default_cubemap",
        "source\\Shaders\\cubemap_shader\\default.vert",
        "source\\Shaders\\cubemap_shader\\default.frag",
       
        ShaderType::CUBEMAP
    );

    auto* cubemap = renderGraph.addPass<CubeMapPass>(
    CubeMapPass::CubeMapPassSettings{
        shaderManager.getShader("default_cubemap"),
        lighting->output(),
        geometry->hardwareDepth(),
    }

);

    shaderManager.load(
        "default_effect",
        "source\\Shaders\\camera_effects\\blur_pass.vert",
        "source\\Shaders\\camera_effects\\blur_pass.frag",
       
        ShaderType::CAMERAEFFECT
    );

    auto* blur = renderGraph.addPass<BlurPass>(BlurPass::BlurPassSettings{ 
                                    shaderManager.getShader("default_effect")
                                    , cubemap->output()
                                    , cubemap->framebuffer()
                                    , geometry->hardwareDepth()
                                    , screenWidth
                                    , screenHeight});

    shaderManager.load(
        "bloom_downsample",
        "source\\Shaders\\bloom_shader\\render_quad.vert",
        "source\\Shaders\\bloom_shader\\bloom_downsample.frag",
 
        ShaderType::CAMERAEFFECT
    );

    shaderManager.load(
        "bloom_upsample",
        "source\\Shaders\\bloom_shader\\render_quad.vert",
        "source\\Shaders\\bloom_shader\\bloom_upsample.frag",
        
        ShaderType::UNKNOWN
    );

    auto* bloom = renderGraph.addPass<BloomPass>(BloomPass::BloomPassSettings{ 
                                      shaderManager.getShader("bloom_downsample")
                                    , shaderManager.getShader("bloom_upsample")
                                    , blur->output()
                                    , screenWidth
                                    , screenHeight});

    shaderManager.load(
        "Composite",
        "source\\Shaders\\composite\\composite.vert",
        "source\\Shaders\\composite\\composite.frag",
        
        ShaderType::UNKNOWN
    );

    auto* composite = renderGraph.addPass<CompositePass>(CompositePass::CompositePassSettings{ 
                                      .shader = shaderManager.getShader("Composite")
                                    , .baseInput = blur->output()
                                    , .inputs = {{bloom->output(),
                                                    "uBloom",
                                                    1}}
                                    , .uniforms = {{ .name = "bloomStrength",
                                                         .value = 0.08f}}
                                    , .width = screenWidth
                                    , .height = screenHeight});

    shaderManager.load(
        "ToneMapping",
        "source\\Shaders\\tonemapping\\ACES.vert",
        "source\\Shaders\\tonemapping\\ACES.frag",
        
        ShaderType::UNKNOWN
    );

    auto* tonemapping = renderGraph.addPass<ToneMappingPass>(ToneMappingPass::ToneMappingPassSettings{ 
                                      shaderManager.getShader("ToneMapping")
                                    , composite->output()
                                    , 0.08f
                                    , screenWidth
                                    , screenHeight});

    shaderManager.load(
        "FXAA",
        "source\\Shaders\\anti_aliasing\\FXAA\\FXAA.vert",
        "source\\Shaders\\anti_aliasing\\FXAA\\FXAA.frag",
        
        ShaderType::UNKNOWN
    );

       auto* FXAA = renderGraph.addPass<FXAAPass>(FXAAPass::FXAAPassSettings{ 
                                      shaderManager.getShader("FXAA")
                                    , tonemapping->output()
                                    , screenWidth
                                    , screenHeight});



        shaderManager.load(
        "default_blit",
        "source\\shaders\\final_blit_shader\\default_final_blit_shader.vert",
        "source\\shaders\\final_blit_shader\\default_final_blit_shader.frag",
        
        ShaderType::BLIT);

        renderGraph.addPass<FinalBlitPass>(FinalBlitPass::FinalBlitPassSettings{ 
                                      shaderManager.getShader("default_blit")
                                    , FXAA->output()
                                    , screenWidth
                                    , screenHeight});
    std::vector<std::string> errors;
    Context().engine.renderer.getRenderGraph()->compile(errors);

    for( auto& str : errors)
            std::cout << str << '\n';

    AddSystem<CameraSystem>(Registry());
    AddSystem<ShadowSystem>(Registry());

    auto& assets = Context().engine.assets;
    auto& registry = Registry();

    assets.textures().initDefaults();

    GetExtractors().push_back( std::make_unique<GlobalTextureExtractor>(
                                assets.textures(), 
                                baker.getBRDF(assets.textures())));
    GetExtractors().push_back(std::make_unique<EnvironmentExtractor>(
                                assets.textures()));
    GetExtractors().push_back(std::make_unique<SceneExtractor>(
                                assets.models(),
                                assets.meshes(),
                                assets.materials(),
                                assets.textures(),
                                assets.skeletons()
    ));
    
    auto skybox = registry.create();

    registry.emplace<CubeMapComponent>(skybox, 
        baker.loadCubeMapHDR(assets.textures(), 
                                "resource\\textures\\hdr\\cedar_bridge_sunset_1_4k.hdr"));
    registry.emplace<TagComponent>(skybox, "CubeMap");

    CameraComponent cameraComp;
    GraphicsEntityFactory::createCamera(registry, Vector3(0, 7.5f, 4), cameraComp, true, "camera");

    assets.models().loadModel("cube",    "resource/models/cube/Box With Spaces.gltf");
    assets.models().loadModel("chess",   "resource/models/chess/chess_set_4k.gltf");
    assets.models().loadModel("map",     "resource/models/map/scene.gltf");
    assets.models().loadModel("duck",    "resource/models/duck/Duck.gltf");
    assets.models().loadModel("helmet",  "resource/models/helmet/DamagedHelmet.gltf");
    assets.models().loadModel("car",     "resource/models/toy car/ToyCar.gltf");
    assets.models().loadModel("boombox", "resource/models/boombox_4k/boombox_4k.gltf");
    assets.models().loadModel("cannon",  "resource/models/cannon_4k.gltf/cannon_01_4k.gltf");
    assets.models().loadModel("animation",  "resource/models/Animation/untitled.gltf");
   //  assets.models().loadModel("animation",  "resource/models/CesiumMan/CesiumMan.gltf");
    auto cube =
        spawnModel(
            "Cube",
            assets.models().getModelID("cube"),
            assets.models(),
            registry
        ).root;

    auto chess =
        spawnModel(
            "Chess",
            assets.models().getModelID("chess"),
            assets.models(),
            registry
        ).root;

    auto map =
        spawnModel(
            "Map",
            assets.models().getModelID("map"),
            assets.models(),
            registry
        ).root;

    auto duck =
        spawnModel(
            "Duckie",
            assets.models().getModelID("duck"),
            assets.models(),
            registry
        ).root;

    auto helmet =
        spawnModel(
            "Helmet",
            assets.models().getModelID("helmet"),
            assets.models(),
            registry
        ).root;

    auto car =
        spawnModel(
            "Car",
            assets.models().getModelID("car"),
            assets.models(),
            registry
        ).root;

    auto boombox =
        spawnModel(
            "BoomBox",
            assets.models().getModelID("boombox"),
            assets.models(),
            registry
        ).root;

    auto cannon =
        spawnModel(
            "Cannon",
            assets.models().getModelID("cannon"),
            assets.models(),
            registry
        ).root;

    auto animation =
        spawnModel(
            "Animation",
            assets.models().getModelID("animation"),
            assets.models(),
            registry
        ).root;

    auto createRBWithModelTransform = [&](entt::entity e, float mass, const Vector3& pos, const Quat& rot, const Vector3& scale) {
        registry.emplace<RigidBodyComponent>(e, PhysicsComponentFactory::createRigidBody(registry, e, pos, rot, scale, mass));
        };

    createRBWithModelTransform(cube,    1.0f, Vector3(0, -2, 0),     Quat(), Vector3(20, 3, 20));
    createRBWithModelTransform(chess,   1.0f, Vector3(-10, 3, -10), Quat(), Vector3(6, 6, 6));
    createRBWithModelTransform(map,     1.0f, Vector3(0, 3, -10),   Quat(), Vector3(0.2f, 0.2f, 0.2f));
    createRBWithModelTransform(duck,    1.0f, Vector3(10, 3, -10),  Quat(), Vector3(1, 1, 1));
    createRBWithModelTransform(helmet,  1.0f, Vector3(-10, 3, 0),   Quat(), Vector3(1, 1, 1));
    createRBWithModelTransform(car,     1.0f, Vector3(0, 3, 0),     Quat(), Vector3(100, 100, 100));
    createRBWithModelTransform(boombox, 1.0f, Vector3(10, 3, 0),    Quat(), Vector3(3, 3, 3));
    createRBWithModelTransform(cannon,  5.0f, Vector3(-10, 3, 10),  Quat(), Vector3(3, 3, 3));
    createRBWithModelTransform(animation,  5.0f, Vector3(0, 10, 0),  Quat(), Vector3(3, 3, 3));
        
    registry.emplace<CollisionShapeComponent>(chess,   PhysicsComponentFactory::createCubeShape(Vector3(3, 3, 3)));
    registry.emplace<CollisionShapeComponent>(map,     PhysicsComponentFactory::createCubeShape(Vector3(0.2f, 0.2f, 0.2f)));
    registry.emplace<CollisionShapeComponent>(duck,    PhysicsComponentFactory::createCubeShape(Vector3(1, 1, 1)));
    registry.emplace<CollisionShapeComponent>(helmet,  PhysicsComponentFactory::createCubeShape(Vector3(1, 1, 1)));
    registry.emplace<CollisionShapeComponent>(car,     PhysicsComponentFactory::createCubeShape(Vector3(100, 100, 100)));
    registry.emplace<CollisionShapeComponent>(boombox, PhysicsComponentFactory::createCubeShape(Vector3(3, 3, 3)));
    registry.emplace<CollisionShapeComponent>(cannon,  PhysicsComponentFactory::createCubeShape(Vector3(3, 3, 3)));

    

   
    registry.emplace<CollisionShapeComponent>(cube, PhysicsComponentFactory::createCubeShape(Vector3(1, 1, 1)));

    LightComponent dir2;
    dir2.type = LightType::Directional;
    dir2.color = Vector3(1, 1, 1);
    dir2.intensity = 8.0f;
    dir2.innerConeAngle = 0.85f;
    dir2.outerConeAngle = 0.90f;
    dir2.castsShadow = true;
    dir2.shadowNearPlane = 1.0f;
    dir2.shadowFarPlane = 150.0f;
    dir2.shadowOrthoSize = 30.0f;
    GraphicsEntityFactory::createLight(
        registry, Vector3(0, 5, 0),
        Quat::fromAxisAngleDeg(Vector3(1.0f, 0.0f, 0.0f), -90.0f),
        dir2, "DirLight");
}
