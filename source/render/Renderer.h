#pragma once

#include "glad/glad.h"

#include "../render/backend/ShaderManager.h"
#include "data/FrameRenderData.h"

#include "backend/ShaderManager.h"
#include "../render/RenderGraph.h"
#include "render/allocator/RenderResourceAllocator.h"

#include "render/data/FrameRenderData.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    void Init();
    void render (const FrameRenderData& frameData) const;
	void rebuildContext(FrameRenderData& ctx);

    const DebugRenderData& getDebugRenderData() const {return debugData; }
	RenderGraph* getRenderGraph() { return &m_RenderGraph;}
    
private:
    ShaderManager* shaderManager;

    DebugRenderData debugData;
   
    RenderResourceAllocator m_ResourceAllocator;
    RenderGraph m_RenderGraph;
};