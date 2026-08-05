// RenderPass.h
#pragma once
#include "../data/FrameRenderData.h"
#include "../data/EngineResources.h"
#include "../data/DebugRenderData.h"
#include "../backend/Shader.h"
#include "../data/RenderResource.h"

class RenderPass
{
protected:
    Shader* shader{nullptr};

public:
    std::vector<RenderResource*> inputs;
    std::vector<RenderResource*> outputs;

    RenderPass(Shader* s) : shader(s) {}

    virtual void execute(const FrameRenderData& frameData, 
                        const EngineResources& resources, 
                        const DebugRenderData& debugData) = 0;
    virtual const char* passName() const { return "RenderPass"; }

    virtual ~RenderPass() = default;
};