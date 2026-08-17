#include "RenderGraph.h"

#include <unordered_set>
#include <deque>
#include <sstream>
#include <variant>

#include "PassResources.h"
#include "resources/assets/Texture.h"
#include "render/backend/containers/FrameBuffer.h"

#include <iostream>

ResourceId RenderGraph::registerRead(PassId pass, ResourceId handle)
{
    auto it = resources.find(handle);
    if (it != resources.end())
        it->second.readers.push_back(pass);
    return handle;
}

ResourceId RenderGraph::registerWrite(PassId pass, ResourceId handle)
{
    ResourceId newHandle = nextResourceId();

    ResourceNode node;

    auto prevIt = resources.find(handle);

    if (prevIt != resources.end())
    {
        node.debugName = prevIt->second.debugName + "'";
        node.desc = prevIt->second.desc;
        node.aliasOf = handle;

        prevIt->second.readers.push_back(pass);
    }
    else
    {
        node.debugName = "unknown";
    }

    node.producer = pass;
    node.imported = false;

    resources[newHandle] = std::move(node);

    return newHandle;
}

ResourceId RenderGraph::registerCreate( PassId pass, const std::string& debugName, const ResourceDesc& desc)
{
    ResourceId handle = nextResourceId();

    ResourceNode node;
    node.debugName = debugName;
    node.desc = desc;
    node.producer = pass;
    node.imported = false;

    resources[handle] = std::move(node);

    return handle;
}

ResourceId RenderGraph::registerImport( PassId pass, const std::string& debugName, void* externalResource)
{
    (void)pass;

    ResourceId handle = nextResourceId();

    ResourceNode node;
    node.debugName = debugName;
    node.producer = INVALID_PASS_ID;
    node.resource = externalResource;
    node.imported = true;

    resources[handle] = std::move(node);

    return handle;
}

bool RenderGraph::compile(std::vector<std::string>& outErrors)
{
    executionOrder.clear();
    outErrors.clear();

    std::vector<RenderPass*> activePasses;
    for (auto& p : passes)
        if (p->active) activePasses.push_back(p.get());

    std::unordered_map<RenderPass*, std::vector<RenderPass*>> adjacency;
    std::unordered_map<RenderPass*, int> inDegree;

    for (RenderPass* p : activePasses) inDegree[p] = 0;

    auto addEdge = [&](RenderPass* from, RenderPass* to)
    {
        adjacency[from].push_back(to);
        inDegree[to]++;
    };

    for (auto& [resId, node] : resources)
    {
        if (node.producer == INVALID_PASS_ID)
            continue; 

        auto producerIt = passIndex.find(node.producer);
        if (producerIt == passIndex.end() || !producerIt->second->active)
            continue; 

        for (PassId readerId : node.readers)
        {
            auto readerIt = passIndex.find(readerId);
            if (readerIt == passIndex.end() || !readerIt->second->active)
                continue;
            addEdge(producerIt->second, readerIt->second);
        }
    }

    for (RenderPass* p : activePasses)
    {
        for (PassId dep : p->orderAfter)
        {
            auto it = passIndex.find(dep);
            if (it == passIndex.end())
            {
                outErrors.push_back("Pass '" + p->name + "' has orderAfter referencing an unknown pass id.");
                continue;
            }
            if (!it->second->active)
                continue; 
            addEdge(it->second, p);
        }
    }

    if (!outErrors.empty())
        return false;

    
    std::deque<RenderPass*> ready;
    for (RenderPass* p : activePasses)
        if (inDegree[p] == 0) ready.push_back(p);

    std::vector<RenderPass*> sorted;
    std::unordered_set<RenderPass*> visited;
    while (!ready.empty())
    {
        RenderPass* current = ready.front();
        ready.pop_front();
        sorted.push_back(current);
        visited.insert(current);

        for (RenderPass* next : adjacency[current])
            if (--inDegree[next] == 0)
                ready.push_back(next);
    }

    if (sorted.size() != activePasses.size())
    {
        std::ostringstream oss;
        oss << "Cycle detected involving passes: ";
        for (RenderPass* p : activePasses)
            if (!visited.count(p))
                oss << "'" << p->name << "' ";
        outErrors.push_back(oss.str());
        return false;
    }

    std::unordered_map<RenderPass*, std::vector<RenderPass*>> reverseAdjacency;
    for (auto& [from, tos] : adjacency)
        for (RenderPass* to : tos)
            reverseAdjacency[to].push_back(from);

    std::unordered_set<RenderPass*> live;
    std::deque<RenderPass*> stack;
    for (RenderPass* p : activePasses)
        if (p->hasSideEffect) { live.insert(p); stack.push_back(p); }

    while (!stack.empty())
    {
        RenderPass* current = stack.front();
        stack.pop_front();
        for (RenderPass* dep : reverseAdjacency[current])
            if (live.insert(dep).second)
                stack.push_back(dep);
    }

    for (RenderPass* p : sorted)
        if (live.count(p))
            executionOrder.push_back(p);

    if (!allocateResources(outErrors))
        return false;
            
    dirty = false;
    return true;
}

void RenderGraph::execute(const FrameRenderData& frameData,
                 const DebugRenderData& debugData) const
{   
    for (RenderPass* p : executionOrder) {
        PassResources passResources(*this, p->id);
        p->execute(frameData, passResources, debugData);
    }
}

bool RenderGraph::allocateResources(
    std::vector<std::string>& outErrors)
{
    // Pass 1: allocate all non-aliased texture resources.
    for (auto& [id, node] : resources)
    {
        if (node.resource != nullptr)
            continue;
        if (node.imported)
            continue;
        if (node.aliasOf != INVALID_RESOURCE_ID)
            continue; // resolved in the aliasing pass below

        if (std::holds_alternative<TextureResourceDesc>(node.desc))
        {
            const auto& desc =
                std::get<TextureResourceDesc>(node.desc);

            Texture* texture =
                m_ResourceAllocator.acquireRenderTarget(
                    desc.width,
                    desc.height,
                    desc.texture
                );

            if (!texture)
            {
                outErrors.push_back(
                    "Failed to allocate texture resource '" +
                    node.debugName + "'."
                );

                return false;
            }

            node.resource = texture;
        }
    }

    for (auto& [id, node] : resources)
    {
        if (node.resource != nullptr)
            continue;
        if (node.aliasOf == INVALID_RESOURCE_ID)
            continue;

        ResourceId root = node.aliasOf;
        std::unordered_set<ResourceId> visitedChain; // cycle guard

        while (true)
        {
            if (!visitedChain.insert(root).second)
            {
                outErrors.push_back(
                    "Alias cycle detected resolving resource '" +
                    node.debugName + "'."
                );
                return false;
            }

            auto it = resources.find(root);
            if (it == resources.end())
            {
                outErrors.push_back(
                    "Resource '" + node.debugName +
                    "' aliases an unknown resource."
                );
                return false;
            }

            if (it->second.aliasOf == INVALID_RESOURCE_ID)
                break;

            root = it->second.aliasOf;
        }

        auto rootIt = resources.find(root);
        if (rootIt == resources.end() || rootIt->second.resource == nullptr)
        {
            outErrors.push_back(
                "Resource '" + node.debugName +
                "' aliases a resource with no allocated backing."
            );
            return false;
        }

        node.resource = rootIt->second.resource;
    }

    // Pass 3: allocate framebuffers, now that all textures they
    // reference (aliased or not) have a resolved physical resource.
    for (auto& [id, node] : resources)
    {
        if (node.resource != nullptr)
            continue;

       if (!std::holds_alternative<FrameBufferResourceDesc>(node.desc))
            continue;

        const auto& desc =
            std::get<FrameBufferResourceDesc>(node.desc);

        FrameBufferDesc framebufferDesc;

        for (ResourceId colorId : desc.colorAttachments)
        {
            auto it = resources.find(colorId);

            if (it == resources.end())
            {
                outErrors.push_back(
                    "Framebuffer '" +
                    node.debugName +
                    "' references unknown color resource."
                );

                return false;
            }

            Texture* texture =
                static_cast<Texture*>(it->second.resource);

            if (!texture)
            {
                outErrors.push_back(
                    "Framebuffer '" +
                    node.debugName +
                    "' references an unallocated color resource."
                );

                return false;
            }

            framebufferDesc.colorAttachments.push_back(texture);
        }

        if (desc.depthAttachment != INVALID_RESOURCE_ID)
        {
            auto it =
                resources.find(desc.depthAttachment);

            if (it == resources.end())
            {
                outErrors.push_back(
                    "Framebuffer '" +
                    node.debugName +
                    "' references unknown depth resource."
                );

                return false;
            }

            Texture* depth =
                static_cast<Texture*>(it->second.resource);

            if (!depth)
            {
                outErrors.push_back(
                    "Framebuffer '" +
                    node.debugName +
                    "' references an unallocated depth resource."
                );

                return false;
            }

            framebufferDesc.depthAttachment = depth;
        }

        FrameBuffer* framebuffer =
            m_ResourceAllocator.acquireFramebuffer(
                framebufferDesc
            );

        if (!framebuffer)
        {
            outErrors.push_back(
                "Failed to allocate framebuffer '" +
                node.debugName +
                "'."
            );

            return false;
        }

        node.resource = framebuffer;
    }

    return true;
}

RenderGraph::ResourceDebugInfo RenderGraph::toDebugInfo(ResourceId id, const ResourceNode& node) const
{
    ResourceDebugInfo info;
    info.id = id;
    info.debugName = node.debugName;
    info.producer = node.producer;
    info.readers = node.readers;
    info.imported = node.imported;
    info.isTexture = std::holds_alternative<TextureResourceDesc>(node.desc);

    if (info.isTexture)
    {
        const auto& desc = std::get<TextureResourceDesc>(node.desc);
        info.width = desc.width;
        info.height = desc.height;

        if (node.resource)
        {
            Texture* tex = static_cast<Texture*>(node.resource);
            info.glTextureID = tex->getID();
        }
    }

    return info;
}

std::vector<RenderGraph::PassDebugInfo> RenderGraph::getPassDebugInfo() const
{
    std::vector<PassDebugInfo> out;
    out.reserve(passes.size());

    std::unordered_set<RenderPass*> inOrder(
        executionOrder.begin(), executionOrder.end()
    );

    for (auto& p : passes)
    {
        PassDebugInfo info;
        info.id = p->id;
        info.name = p->passName();
        info.active = p->active;
        info.hasSideEffect = p->hasSideEffect;
        info.inExecutionOrder = inOrder.count(p.get()) > 0;
        out.push_back(std::move(info));
    }

    return out;
}

std::vector<RenderGraph::ResourceDebugInfo> RenderGraph::getResourceDebugInfo() const
{
    std::vector<ResourceDebugInfo> out;
    out.reserve(resources.size());

    for (auto& [id, node] : resources)
        out.push_back(toDebugInfo(id, node));

    return out;
}

std::vector<RenderGraph::ResourceDebugInfo> RenderGraph::getResourcesForPass(PassId id) const
{
    std::vector<ResourceDebugInfo> out;

    for (auto& [resId, node] : resources)
    {
        bool isProducer = node.producer == id;
        bool isReader =
            std::find(node.readers.begin(), node.readers.end(), id) != node.readers.end();

        if (isProducer || isReader)
            out.push_back(toDebugInfo(resId, node));
    }

    return out;
}