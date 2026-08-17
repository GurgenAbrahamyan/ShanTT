#include "RenderGraphBuilder.h"
#include "RenderGraph.h"

ResourceId RenderGraphBuilder::read(ResourceId handle)
{
    return graph.registerRead(passId, handle);
}

ResourceId RenderGraphBuilder::write(ResourceId handle)
{
    return graph.registerWrite(passId, handle);
}

ResourceId RenderGraphBuilder::create(const std::string& debugName, const ResourceDesc& desc)
{
    return graph.registerCreate(passId, debugName, desc);
}

ResourceId RenderGraphBuilder::import(const std::string& debugName, void* externalResource)
{
    return graph.registerImport(passId, debugName, externalResource);
}