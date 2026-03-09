    #include "Renderer.h"
    #include "../math_custom/GLAdapter.h"




    using namespace GLAdapter;

    Renderer::Renderer(EventBus* bus)
        : window(nullptr),
        bus(bus),
        shaderManager(nullptr)
  
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
            EngineContext::get().windowWidth,
            EngineContext::get().windowHeight,
            "LearnOpenGL",
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
        EngineContext::get().setWindow(window);

        m_MainFrameBuffer = std::make_unique<FrameBuffer>(
            EngineContext::get().windowWidth,
            EngineContext::get().windowHeight
        );

		m_MainFrameBuffer->addColorBuffer();
		m_MainFrameBuffer->addDepthBuffer();

        m_BlurFrameBuffer = std::make_unique<FrameBuffer>(
            EngineContext::get().windowWidth,
            EngineContext::get().windowHeight
		);
        m_BlurFrameBuffer->addColorBuffer();
		m_BlurFrameBuffer->addDepthBuffer();

        m_ShadowFrameBuffer = std::make_unique<FrameBuffer>(
			4096, 4096
		);

		m_ShadowFrameBuffer->addDepthBuffer();

        m_ShadowFrameBuffer->bind();
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        m_ShadowFrameBuffer->disableColor();
        m_ShadowFrameBuffer->unbind();
		
		graph = new RenderGraph();
		
      
        shadowResource = new RenderResource();
        shadowResource->framebuffer = m_ShadowFrameBuffer.get();
        sceneResource = new RenderResource();
        sceneResource->framebuffer = m_MainFrameBuffer.get();
        blurResource = new RenderResource();
        blurResource->framebuffer = m_BlurFrameBuffer.get();


       shaderManager->load(
            "default_shadow",
            "resource\\Shaders\\shadow_shader\\shadow_pass.vert",
            "resource\\Shaders\\shadow_shader\\shadow_pass.frag",
            "resource\\Shaders\\object_shader\\default.geom",
            ShaderType::SHADOWMAP
        );


        shaderManager->load(
            "default",
            "resource\\Shaders\\object_shader\\default.vert",
            "resource\\Shaders\\object_shader\\default.frag",
            "resource\\Shaders\\object_shader\\default.geom",
            ShaderType::OBJECT3D
        );

        Shader* objShader = shaderManager->getShader("default");
        objShader->Activate();
        GLuint blockIndex = glGetUniformBlockIndex(objShader->ID, "LightBlock");
        glUniformBlockBinding(objShader->ID, blockIndex, 1);


        shaderManager->load(
            "default_cubemap",
            "resource\\Shaders\\cubemap_shader\\default.vert",
            "resource\\Shaders\\cubemap_shader\\default.frag",
            "resource\\Shaders\\object_shader\\default.geom",
            ShaderType::CUBEMAP
        );

        shaderManager->load(
            "default_effect",
            "resource\\Shaders\\camera_effects\\blur_pass.vert",
            "resource\\Shaders\\camera_effects\\blur_pass.frag",
            "resource\\Shaders\\object_shader\\default.geom",
            ShaderType::CAMERAEFFECT
        );


        shaderManager->load(
            "default_blit",
            "resource\\Shaders\\final_blit_shader\\default_final_blit_shader.vert",
            "resource\\Shaders\\final_blit_shader\\default_final_blit_shader.frag",
            "resource\\Shaders\\object_shader\\default.geom",
            ShaderType::BLIT
        );



        auto* shadow = graph->addPass<ShadowPass>(
            shaderManager->getShader("default_shadow"), 4096);
            

        auto* geometry = graph->addPass<GeometryPass>(
            shaderManager->getShader("default"));

        auto* cubemap = graph->addPass<CubeMapPass>(
            shaderManager->getShader("default_cubemap"));

        auto* blur = graph->addPass<BlurPass>(
            shaderManager->getShader("default_effect"));
            
        auto* finalPass = graph->addPass<FinalBlitPass>(
            shaderManager->getShader("default_blit"));


        
       

        shadow->outputs.push_back(shadowResource);

        geometry->inputs.push_back(shadowResource);
        geometry->outputs.push_back(sceneResource);

        cubemap->outputs.push_back(sceneResource);

        blur->inputs.push_back(sceneResource);
        blur->outputs.push_back(blurResource);
        
        finalPass->inputs.push_back(blurResource);


    
        
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








    void Renderer::render(RenderContext* ctx)
    {
        clearFramebuffers();

        graph->execute(*ctx);

        glfwSwapBuffers(window);
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
            for (auto submesh : view.get<ModelComponent>(entity).meshes)
            {
                Mat4 entityWorld = GLAdapter::toGL(getWorldTransform(entity, registry));
                auto& transform = view.get<TransformComponent>(entity);
                auto& meshComp = submesh.mesh;
                auto& matComp = submesh.material;
                if (!meshComp || !matComp)
                    continue;
                Mat4 model = submesh.localTransform; // or world matrix
                auto& meshMap = ctx->batches[matComp];
                auto& batch = meshMap[meshComp];
                batch.instances.push_back(model*entityWorld);
            }

        }

        ctx->lights.clear();
		int shadowIndex = 0;
        registry.view<LightComponent, TransformComponent>().each(
            [&](entt::entity entity, LightComponent& lc, TransformComponent& tc)
            {
                GPULight l{};

                l.type = static_cast<int>(lc.type);
                if(lc.castsShadow) {
                    l.shadowIndex = shadowIndex++;
				}
                l.intensity = lc.intensity;
                l.color = lc.color;
                l.position = GLAdapter::toGL(tc.position);
                l.direction = GLAdapter::toGL(lc.direction);
                l.innerCone = lc.innerConeAngle;
                l.outerCone = lc.outerConeAngle;

                ctx->lights.push_back(l);
            });
    };

    Mat4 Renderer::getWorldTransform(entt::entity entity, entt::registry& registry) {

        Mat4 local = Mat4();

        if (auto* tc = registry.try_get<TransformComponent>(entity)) {
            local =
                Mat4::scale(tc->scale) *
                Mat4::fromQuat(tc->rotation) *
                Mat4::translate(tc->position)
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
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | 0x00000100);
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
			glViewport(0, 0, EngineContext::get().windowWidth, EngineContext::get().windowHeight);
            m_ShadowFrameBuffer->unbind();
        }

      
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }