#include "Renderer.h"

#include "handlers/ShadowPass.h"
#include "handlers/GeometryPass.h"
#include "handlers/LightingPass.h"
#include "handlers/CubeMapPass.h"
#include "handlers/BlurPass.h"
#include "handlers/BloomPass.h"
#include "handlers/FinalBlitPass.h"
#include "handlers/FXAAPass.h"


#include <vector>
#include "data/ShaderType.h"



Renderer::Renderer( EventBus* bus, EngineResources* ctx) :
    bus(bus),
    shaderManager(nullptr),
    ctx(ctx),
    blurResource(nullptr),
    shadowResource(nullptr),
    lightResource(nullptr),
    sceneResource(nullptr),
    graph(nullptr)
{

    shaderManager = new ShaderManager(bus);

   


    m_ShadowFrameBuffer = std::make_unique<FrameBuffer>(
        1024, 1024
    );

    m_ShadowFrameBuffer->addDepthBuffer();

    m_ShadowFrameBuffer->bind();
    
    m_ShadowFrameBuffer->disableColor();
    m_ShadowFrameBuffer->unbind();

    auto fbWidth { static_cast<uint32_t>(ctx->windowSize.x) };
    auto fbHeight { static_cast<uint32_t>(ctx->windowSize.y) };
     
    m_MainFrameBuffer = std::make_unique<FrameBuffer>(
        fbWidth,
        fbHeight
    );

    m_MainFrameBuffer->addColorBuffer(GL_RGBA8, GL_RGBA, GL_FLOAT);  // gAlbedo
    m_MainFrameBuffer->addColorBuffer(GL_R32F, GL_RED, GL_FLOAT);   // gDepth
    m_MainFrameBuffer->addColorBuffer(GL_RG16F, GL_RG, GL_FLOAT);   // gNormal
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

    shaderManager->load(
        "FXAA",
        "resource\\Shaders\\anti_aliasing\\FXAA\\FXAA.vert",
        "resource\\Shaders\\anti_aliasing\\FXAA\\FXAA.frag",
        
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
    auto* fxaaPass = graph->addPass<FXAAPass>(
        shaderManager->getShader("FXAA")
    );





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

    RenderResource* res = new RenderResource();
    res->framebuffer = new FrameBuffer(fbWidth, fbHeight); //TO-DO FIX
    res->framebuffer->addColorBuffer();
    finalPass->outputs.push_back(res);

    fxaaPass->inputs.push_back(res);




    
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_MULTISAMPLE);
    glDepthFunc(GL_LESS);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

   
}

Renderer::~Renderer() {

    delete shaderManager;
}



void Renderer::render(const FrameRenderData& frameData) const
{
    clearFramebuffers();

    graph->execute(frameData, *ctx , debugData );

}

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

void Renderer::clearFramebuffers() const
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
        glViewport(0, 0, ctx->windowSize.x, ctx->windowSize.y);
        m_ShadowFrameBuffer->unbind();
    }

    if (m_LightFrameBuffer) {

        m_LightFrameBuffer->bind();
        glViewport(0, 0, m_LightFrameBuffer->getWidth(), m_LightFrameBuffer->getHeight());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        m_LightFrameBuffer->unbind();


    }



    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.67f, 0.67f, 0.67f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}