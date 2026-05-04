#include "Renderer.h"

#include "handlers/ShadowPass.h"
#include "handlers/GeometryPass.h"
#include "handlers/LightingPass.h"
#include "handlers/CubeMapPass.h"
#include "handlers/BlurPass.h"
#include "handlers/BloomPass.h"
#include "handlers/FinalBlitPass.h"





Renderer::Renderer(EventBus* bus, RenderContext* ctx)
    : window(nullptr),
    bus(bus),
    shaderManager(nullptr),
    ctx(ctx)

{

    shaderManager = new ShaderManager(bus);

    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW\n";
        return;
    }

    int samples = 8;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, samples);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(
        ctx->windowWidth,
        ctx->windowHeight,
        "Hexium",
        nullptr,
        nullptr
    );


    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return;
    }

    ui = new UiInput(window, bus);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    EngineContext::get().setWindow(window);

    m_ShadowFrameBuffer = std::make_unique<FrameBuffer>(
        1024, 1024
    );

    m_ShadowFrameBuffer->addDepthBuffer();

    m_ShadowFrameBuffer->bind();
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_ShadowFrameBuffer->disableColor();
    m_ShadowFrameBuffer->unbind();


    m_MainFrameBuffer = std::make_unique<FrameBuffer>(
        fbWidth,
        fbHeight
    );

    m_MainFrameBuffer->addColorBuffer(GL_RGBA16F, GL_RGBA, GL_FLOAT);  // gAlbedo
    m_MainFrameBuffer->addColorBuffer(GL_RGBA32F, GL_RGBA, GL_FLOAT);   // gPos
    m_MainFrameBuffer->addColorBuffer(GL_RGBA32F, GL_RGBA, GL_FLOAT);   // gNormal
    m_MainFrameBuffer->addColorBuffer(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE); // gARM
    m_MainFrameBuffer->addColorBuffer(GL_RGBA16F, GL_RGBA, GL_FLOAT);   // gEmissive
    m_MainFrameBuffer->addDepthBuffer();

    m_BlurFrameBuffer = std::make_unique<FrameBuffer>(
        fbWidth,
        fbHeight
    );

    m_LightFrameBuffer = std::make_unique<FrameBuffer>(
        fbWidth,
        fbHeight
    );

    m_LightFrameBuffer->addColorBuffer(GL_RGBA16F, GL_RGBA, GL_FLOAT);
    m_LightFrameBuffer->addDepthBuffer();

    m_BlurFrameBuffer->addColorBuffer();
    m_BlurFrameBuffer->addDepthBuffer();






    graph = new RenderGraph();


    shadowResource = new RenderResource();
    shadowResource->framebuffer = m_ShadowFrameBuffer.get();
    sceneResource = new RenderResource();
    sceneResource->framebuffer = m_MainFrameBuffer.get();
    blurResource = new RenderResource();
    blurResource->framebuffer = m_BlurFrameBuffer.get();
    lightResource = new RenderResource();
    lightResource->framebuffer = m_LightFrameBuffer.get();
	

    shaderManager->load(
        "default_shadow",
        "resource\\Shaders\\shadow_shader\\shadow_pass.vert",
        "resource\\Shaders\\shadow_shader\\shadow_pass.frag",
        
        ShaderType::SHADOWMAP
    );


    shaderManager->load(
        "default",
        "resource\\Shaders\\object_shader\\default.vert",
        "resource\\Shaders\\object_shader\\default.frag",
        
        ShaderType::OBJECT3D
    );

    shaderManager->load(
        "default_light",
        "resource\\shaders\\lighting_shader\\lighting_pass.vert",
        "resource\\shaders\\lighting_shader\\lighting_pass.frag",
       
        ShaderType::LIGHT
    );

    Shader* objShader = shaderManager->getShader("default_light");
    objShader->Activate();
    GLuint blockIndex = glGetUniformBlockIndex(objShader->ID, "LightBlock");
    glUniformBlockBinding(objShader->ID, blockIndex, 1);


    shaderManager->load(
        "default_cubemap",
        "resource\\Shaders\\cubemap_shader\\default.vert",
        "resource\\Shaders\\cubemap_shader\\default.frag",
       
        ShaderType::CUBEMAP
    );

    shaderManager->load(
        "default_effect",
        "resource\\Shaders\\camera_effects\\blur_pass.vert",
        "resource\\Shaders\\camera_effects\\blur_pass.frag",
       
        ShaderType::CAMERAEFFECT
    );


    shaderManager->load(
        "default_blit",
        "resource\\Shaders\\final_blit_shader\\default_final_blit_shader.vert",
        "resource\\Shaders\\final_blit_shader\\default_final_blit_shader.frag",
        
        ShaderType::BLIT
    );

    shaderManager->load(
        "bloom_downsample",
        "resource\\Shaders\\bloom_shader\\render_quad.vert",
        "resource\\Shaders\\bloom_shader\\bloom_downsample.frag",
 
        ShaderType::CAMERAEFFECT
    );

    shaderManager->load(
        "bloom_upsample",
        "resource\\Shaders\\bloom_shader\\render_quad.vert",
        "resource\\Shaders\\bloom_shader\\bloom_upsample.frag",
        
        ShaderType::UNKNOWN
    );



    auto* shadow = graph->addPass<ShadowPass>(
        shaderManager->getShader("default_shadow"));


    auto* geometry = graph->addPass<GeometryPass>(
        shaderManager->getShader("default"));
    auto* lighting = graph->addPass<LightingPass>(
        shaderManager->getShader("default_light"));

    auto* cubemap = graph->addPass<CubeMapPass>(
        shaderManager->getShader("default_cubemap"));

    auto* blur = graph->addPass<BlurPass>(
        shaderManager->getShader("default_effect"));

    auto* bloom = graph->addPass<BloomPass>(
        shaderManager->getShader("bloom_downsample"),
        shaderManager->getShader("bloom_upsample"),
        fbWidth, fbHeight);
    auto* finalPass = graph->addPass<FinalBlitPass>(
        shaderManager->getShader("default_blit"));





    shadow->outputs.push_back(shadowResource);

    geometry->outputs.push_back(sceneResource);

    lighting->inputs.push_back(sceneResource);
    lighting->inputs.push_back(shadowResource);
    lighting->outputs.push_back(lightResource);

    cubemap->inputs.push_back(sceneResource);
    cubemap->outputs.push_back(lightResource);


    bloom->inputs.push_back(lightResource);
    bloom->outputs.push_back(new RenderResource());


    blur->inputs.push_back(lightResource);
    blur->outputs.push_back(blurResource);



 
    finalPass->inputs.push_back(blurResource);
    finalPass->inputs.push_back(bloom->outputs[0]);





    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_MULTISAMPLE);
    glDepthFunc(GL_LESS);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

Renderer::~Renderer() {

    delete shaderManager;

    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}








void Renderer::render()
{
    clearFramebuffers();

    graph->execute(*ctx);



    ui->render();

}



GLFWwindow* Renderer::getWindow() const {
    return window;
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Renderer::rebuildContext(RenderContext* ctx)
{




    if (!ctx || !ctx->registry) return;


    auto& registry = *ctx->registry;



    ctx->camera = nullptr;
    ctx->cameraTransform = nullptr;
    ctx->cubeMapComp = nullptr;

    // find active camera
    for (auto entity : registry.view<ActiveCameraTag>())
    {

        ctx->camera = registry.try_get<CameraComponent>(entity);
        ctx->cameraTransform = registry.try_get<TransformComponent>(entity);
        break;
    }

    ctx->batches.clear();

    auto view = registry.view<
        TransformComponent,
        ModelComponent>();

    for (auto entity : view)
    {
        for (auto submesh : view.get<ModelComponent>(entity).asset->meshes)
        {
            Mat4 entityWorld = getWorldTransform(entity, registry);
            auto& transform = view.get<TransformComponent>(entity);
            auto& meshComp = submesh.mesh;
            auto& matComp = submesh.material;
            if (!meshComp || !matComp)
                continue;
            Mat4 model = submesh.localTransform;
            auto& meshMap = ctx->batches[matComp];
            auto& batch = meshMap[meshComp];
            batch.instances.push_back(entityWorld*model);
        }

    }

    ctx->lights.clear();
    int shadowIndex = 0;
    registry.view<LightComponent, TransformComponent>().each(
        [&](entt::entity entity, LightComponent& lc, TransformComponent& tc)
        {
            GPULight l{};

            l.type = static_cast<int>(lc.type);
            if (lc.castsShadow) {

                l.shadowIndex = shadowIndex;
                if (lc.type == LightType::Point) {
                    shadowIndex += 6;
                }
                else {
                    shadowIndex += 1;
                }
            }
            l.intensity = lc.intensity;
            l.color = lc.color;
            l.position = tc.position;
            l.direction = lc.direction;
            l.innerCone = lc.innerConeAngle;
            l.outerCone = lc.outerConeAngle;

            ctx->lights.push_back(l);
        });

    auto skyView = registry.view<CubeMapComponent>();
    if (skyView.empty()) return;
    ctx->cubeMapComp = &registry.get<CubeMapComponent>(skyView.front());

    ctx->debugTextures.clear();
    if (m_MainFrameBuffer) {
        ctx->debugTextures.push_back({ "Albedo",    m_MainFrameBuffer->getColorAttachment(0) });
        ctx->debugTextures.push_back({ "Position",  m_MainFrameBuffer->getColorAttachment(1) });
        ctx->debugTextures.push_back({ "Normal",    m_MainFrameBuffer->getColorAttachment(2) });
        ctx->debugTextures.push_back({ "ARM",       m_MainFrameBuffer->getColorAttachment(3) });
        ctx->debugTextures.push_back({ "Emissive",  m_MainFrameBuffer->getColorAttachment(4) });
    }
    if (m_LightFrameBuffer)
        ctx->debugTextures.push_back({ "Light Pass", m_LightFrameBuffer->getColorAttachment(0) });
    if (m_BlurFrameBuffer)
        ctx->debugTextures.push_back({ "Blur / Final", m_BlurFrameBuffer->getColorAttachment(0) });
    if (m_ShadowFrameBuffer)
        ctx->debugTextures.push_back({ "Shadow Depth", m_ShadowFrameBuffer->getDepthAttachment() });

    ui->startNewFrame();

    ui->buildUI(ctx, graph);
};

Mat4 Renderer::getWorldTransform(entt::entity entity, entt::registry& registry) {

    Mat4 local = Mat4();

    if (auto* tc = registry.try_get<TransformComponent>(entity)) {
        local =
            Mat4::translate(tc->position) *
            Mat4::fromQuat(tc->rotation) *
            Mat4::scale(tc->scale)


            ;
    }


    if (auto* parentComp = registry.try_get<ParentComponent>(entity)) {
        if (registry.valid(parentComp->parent)) {
            return getWorldTransform(parentComp->parent, registry) * local;
        }
    }

    return  local;
}

void Renderer::clearFramebuffers()
{

    if (m_MainFrameBuffer) {
        m_MainFrameBuffer->bind();
        glViewport(0, 0, m_MainFrameBuffer->getWidth(), m_MainFrameBuffer->getHeight());
        glClearColor(0.67f, 0.67f, 0.67f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_MainFrameBuffer->unbind();
    }


    if (m_BlurFrameBuffer) {
        m_BlurFrameBuffer->bind();
        glViewport(0, 0, m_BlurFrameBuffer->getWidth(), m_BlurFrameBuffer->getHeight());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_BlurFrameBuffer->unbind();
    }


    if (m_ShadowFrameBuffer) {
        m_ShadowFrameBuffer->bind();
        glViewport(0, 0, m_ShadowFrameBuffer->getWidth(), m_ShadowFrameBuffer->getHeight());
        glClearDepth(1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, ctx->windowWidth, ctx->windowHeight);
        m_ShadowFrameBuffer->unbind();
    }

    if (m_LightFrameBuffer) {

        m_LightFrameBuffer->bind();
        glViewport(0, 0, m_LightFrameBuffer->getWidth(), m_LightFrameBuffer->getHeight());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ? depth never cleared!
        m_LightFrameBuffer->unbind();


    }



    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.67f, 0.67f, 0.67f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}