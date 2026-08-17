#pragma once
#include "handlers/RenderId.h"
#include "RenderGraph.h"

class PassResources
{
public:
    template<typename T>
    T* get(ResourceId handle) const
    {
        return graph.resolve<T>(handle, owningPass);
    }

private:
    friend class RenderGraph;
    PassResources(const RenderGraph& g, PassId owner) : graph(g), owningPass(owner) {}

    const RenderGraph& graph;
    PassId owningPass;
};