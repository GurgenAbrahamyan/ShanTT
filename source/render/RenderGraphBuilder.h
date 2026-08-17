#pragma once
#include <string>
#include <cassert>
#include "handlers/RenderId.h"
#include "render/allocator/RenderResource.h"

class RenderGraph;

class RenderGraphBuilder
{
public:
    ResourceId read(ResourceId handle);
    ResourceId write(ResourceId handle);
    ResourceId create(const std::string& debugName, const ResourceDesc& desc);
    ResourceId import(const std::string& debugName, void* externalResource);

    PassId currentPass() const { return passId; }

private:
    friend class RenderGraph;
    explicit RenderGraphBuilder(RenderGraph& g, PassId id) : graph(g), passId(id) {}

    RenderGraph& graph;
    const PassId passId;
};