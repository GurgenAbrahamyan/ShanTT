#pragma once
#include <vector>
#include <string>
#include "RenderId.h"
#include "../RenderGraphBuilder.h"
#include "../data/FrameRenderData.h"
#include "../data/DebugRenderData.h"

class PassResources;

class RenderPass
{
public:
    const PassId id;
    std::string name;

    RenderPass(RenderGraphBuilder& builder, std::string passName)
        : id(builder.currentPass()), name(std::move(passName)){}

    virtual void execute(const FrameRenderData& frameData,
                          PassResources& resources,
                          const DebugRenderData& debugData) = 0;

    virtual const char* passName() const { return name.c_str(); }

    bool hasSideEffect = false;
    bool active = true;
    std::vector<PassId> orderAfter;

    virtual ~RenderPass() = default;
};