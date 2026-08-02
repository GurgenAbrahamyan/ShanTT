#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "../math_custom/Mat4.h"
#include "backend/Shader.h"

#include "backend/containers/FrameBuffer.h"


#include "../render/backend/ShaderManager.h"
#include "../core/EventBus.h"


#include "data/RenderContext.h"


#include "backend/ShaderManager.h"
#include "../render/RenderGraph.h"

#include "../input/UiInput.h"
class Scene;
class Camera;

class Renderer {
public:
    Renderer(EventBus* bus, RenderContext* ctx);
    ~Renderer();

    void render();
	FrameBuffer* getMainFrameBuffer() const { return m_MainFrameBuffer.get(); }
	FrameBuffer* getBlurFrameBuffer() const { return m_BlurFrameBuffer.get(); }
	FrameBuffer* getShadowFrameBuffer() const { return m_ShadowFrameBuffer.get(); }
	void rebuildContext(RenderContext* ctx);
	RenderGraph* getRenderGraph() const { return graph; }

    GLFWwindow* getWindow() const;

    
    

private:

	Mat4 getWorldTransform(entt::entity entity, entt::registry& registry);
    void clearFramebuffers();
    GLFWwindow* window;
   [[maybe_unused]] EventBus* bus;
    ShaderManager* shaderManager;

    RenderContext *ctx;
     UiInput* ui;

    
    std::unique_ptr<FrameBuffer> m_MainFrameBuffer;
	std::unique_ptr<FrameBuffer> m_BlurFrameBuffer;
	std::unique_ptr<FrameBuffer> m_ShadowFrameBuffer;
    std::unique_ptr<FrameBuffer> m_LightFrameBuffer;


    RenderResource* blurResource;
    RenderResource *shadowResource;
    RenderResource* lightResource;
    RenderResource* sceneResource;
   
    RenderGraph* graph;
   
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};