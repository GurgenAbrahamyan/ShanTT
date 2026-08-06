// RenderGraph.h
#pragma once
#include <vector>
#include <memory>
#include <string>

#include "handlers/RenderPass.h"
#include "data/FrameRenderData.h"
#include "data/EngineResources.h"
#include "data/DebugRenderData.h"

class RenderGraph
{
public:
    template<typename T, typename... Args>
    T* addPass(Args&&... args)
    {
        auto pass = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = pass.get();
        passes.emplace_back(std::move(pass));
        dirty = true;
        return ptr;
    }

   
    bool compile(std::vector<std::string>& outErrors);

    void execute(const FrameRenderData& frameData,
                 const EngineResources& resources,
                 const DebugRenderData& debugData)
    {
        for (RenderPass* p : executionOrder)
            p->execute(frameData, resources, debugData);
    }

    const std::vector<std::unique_ptr<RenderPass>>& getPasses() const { return passes; }

private:
    std::vector<std::unique_ptr<RenderPass>> passes;
    std::vector<RenderPass*> executionOrder; 
    bool dirty = true;
};