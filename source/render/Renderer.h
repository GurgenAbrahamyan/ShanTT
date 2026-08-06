#pragma once

#include "glad/glad.h"

#include "../math_custom/Mat4.h"
#include "backend/Shader.h"


#include "../render/backend/ShaderManager.h"
#include "../core/EventBus.h"


#include "data/EngineResources.h"
#include "data/FrameRenderData.h"

#include "backend/ShaderManager.h"
#include "../render/RenderGraph.h"

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
    
private:

	Mat4 getWorldTransform(entt::entity entity, entt::registry& registry);

    void clearFramebuffers() const;

   [[maybe_unused]] EventBus* bus;
    ShaderManager* shaderManager;

    EngineResources* ctx;
    DebugRenderData debugData;
   
    RenderGraph* graph;
   
    
};