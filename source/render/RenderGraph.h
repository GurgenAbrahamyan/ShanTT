#pragma once
#include <vector>
#include <memory>
#include "handlers/RenderPass.h"
#include "data/FrameRenderData.h"
#include "data/EngineResources.h"
#include "data/DebugRenderData.h"

class RenderGraph
{
private:
    std::vector<std::unique_ptr<RenderPass>> passes;

public:
    template<typename T, typename... Args>
    T* addPass(Args&&... args)
    {
        T* pass = new T(std::forward<Args>(args)...);
        passes.emplace_back(pass);
        return pass;
    }

    void execute(const FrameRenderData& frameData, const EngineResources& resources, const DebugRenderData& debugData)
    {
        for (auto& p : passes)
            p->execute(frameData, resources, debugData);
    }

    const std::vector<std::unique_ptr<RenderPass>>& getPasses() const { return passes; }
};