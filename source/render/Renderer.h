#pragma once

#include "glad/glad.h"

#include "../math_custom/Mat4.h"
#include "backend/Shader.h"

#include "backend/containers/FrameBuffer.h"


#include "../render/backend/ShaderManager.h"
#include "../core/EventBus.h"


#include "data/EngineResources.h"
#include "data/FrameRenderData.h"

#include "backend/ShaderManager.h"
#include "../render/RenderGraph.h"

#include "../input/UiInput.h"
class Scene;
class Camera;

class Renderer {
public:
    Renderer(EventBus* bus, EngineResources* ctx);
    ~Renderer();

    void render (const FrameRenderData& frameData) const;
	void rebuildContext(FrameRenderData& ctx);

    const DebugRenderData& getDebugRenderData() const {return debugData; }
	RenderGraph* getRenderGraph() const { return graph; }
    FrameBuffer* getMainFrameBuffer() const { return m_MainFrameBuffer.get(); }
	FrameBuffer* getBlurFrameBuffer() const { return m_BlurFrameBuffer.get(); }
	FrameBuffer* getShadowFrameBuffer() const { return m_ShadowFrameBuffer.get(); }
    
private:

	Mat4 getWorldTransform(entt::entity entity, entt::registry& registry);

    void clearFramebuffers() const;

   [[maybe_unused]] EventBus* bus;
    ShaderManager* shaderManager;

    EngineResources* ctx;
    DebugRenderData debugData;
    
    std::unique_ptr<FrameBuffer> m_MainFrameBuffer;
	std::unique_ptr<FrameBuffer> m_BlurFrameBuffer;
	std::unique_ptr<FrameBuffer> m_ShadowFrameBuffer;
    std::unique_ptr<FrameBuffer> m_LightFrameBuffer;


    RenderResource* blurResource;
    RenderResource *shadowResource;
    RenderResource* lightResource;
    RenderResource* sceneResource;
   
    RenderGraph* graph;
   
};