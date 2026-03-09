#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "../math_custom/Mat4.h"
#include "backend/Shader.h"
#include "../core/EngineContext.h"
#include "backend/containers/FrameBuffer.h"


#include "../render/backend/ShaderManager.h"
#include "../core/EventBus.h"

#include "data/BatchMap.h"
#include "data/MeshBatch.h"
#include "data/ShaderType.h"
#include "data/RenderContext.h"

#include "handlers/RenderPass.h"
#include "backend/ShaderManager.h"
#include "../render/RenderGraph.h"

class Scene;
class Camera;

class Renderer {
public:
    Renderer(EventBus* bus);
    ~Renderer();

    void render(RenderContext* ctx);
	FrameBuffer* getMainFrameBuffer() const { return m_MainFrameBuffer.get(); }
	FrameBuffer* getBlurFrameBuffer() const { return m_BlurFrameBuffer.get(); }
	FrameBuffer* getShadowFrameBuffer() const { return m_ShadowFrameBuffer.get(); }
	void rebuildContext(RenderContext* ctx);


    GLFWwindow* getWindow() const;

    
    

private:

	Mat4 getWorldTransform(entt::entity entity, entt::registry& registry);
    void clearFramebuffers();
    GLFWwindow* window;
   
    ShaderManager* shaderManager;
    EventBus* bus;
    
    std::unique_ptr<FrameBuffer> m_MainFrameBuffer;
	std::unique_ptr<FrameBuffer> m_BlurFrameBuffer;
	std::unique_ptr<FrameBuffer> m_ShadowFrameBuffer;
    std::vector<std::unique_ptr<RenderPass>> renderHandlers;

    RenderGraph* graph;

    RenderResource* shadowResource;
    RenderResource* sceneResource;
    RenderResource* blurResource;
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};