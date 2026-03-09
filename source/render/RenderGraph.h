#pragma once
#include <vector>
#include <memory>
#include "handlers/RenderPass.h"
#include "data/RenderContext.h"
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

    void execute(RenderContext& ctx)
    {
        for (auto& p : passes)
            p->execute(ctx);
    }
};