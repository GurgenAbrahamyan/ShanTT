#pragma once
#include <vector>
#include <string>
#include "RenderId.h"
#include "../data/FrameRenderData.h"
#include "../data/EngineResources.h"
#include "../data/DebugRenderData.h"
#include "../backend/Shader.h"

class RenderPass
{
protected:
    Shader* shader{nullptr}; 

public:
    const PassId id;
    std::string name;
    std::vector<ResourceId> inputs;
    std::vector<ResourceId> outputs;
    std::vector<PassId> orderAfter; 

    RenderPass(Shader* s, std::string passName)
        : shader(s), id(nextPassId()), name(std::move(passName)) {}

    virtual void execute(const FrameRenderData& frameData,
                          const EngineResources& resources,
                          const DebugRenderData& debugData) = 0;

    virtual const char* passName() const { return "RenderPass"; }

    virtual ~RenderPass() = default;
};