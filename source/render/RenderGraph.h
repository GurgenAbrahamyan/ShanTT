#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "handlers/RenderPass.h"
#include "RenderGraphBuilder.h"
#include "data/FrameRenderData.h"
#include "data/DebugRenderData.h"

#include "render/allocator/RenderResourceAllocator.h"

class RenderGraph
{
    public:
    struct PassDebugInfo
    {
        PassId id = INVALID_PASS_ID;
        std::string name;
        bool active = false;
        bool hasSideEffect = false;
        bool inExecutionOrder = false;
    };

    struct ResourceDebugInfo
    {
        ResourceId id = INVALID_RESOURCE_ID;
        std::string debugName;
        PassId producer = INVALID_PASS_ID;
        std::vector<PassId> readers;
        bool imported = false;
        bool isTexture = false;
        unsigned int glTextureID = 0;
        uint32_t width = 0;
        uint32_t height = 0;
    };

    const std::vector<RenderPass*>& getExecutionOrder() const { return executionOrder; }

    std::vector<PassDebugInfo> getPassDebugInfo() const;
    std::vector<ResourceDebugInfo> getResourceDebugInfo() const;
    std::vector<ResourceDebugInfo> getResourcesForPass(PassId id) const;

public:
  
    RenderGraph(RenderResourceAllocator& allocator) : m_ResourceAllocator(allocator) {}
    template<typename T, typename... Args>
    T* addPass(Args&&... args)
    {
        PassId id = nextPassId();
        RenderGraphBuilder builder(*this, id);

        auto pass = std::make_unique<T>(builder, std::forward<Args>(args)...);
        T* ptr = pass.get();

        passes.emplace_back(std::move(pass));
        passIndex[id] = passes.back().get();
        dirty = true;
        return ptr;
    }

    void setPassActive(PassId id, bool isActive)
    {
        
        if (auto it = passIndex.find(id); it != passIndex.end()) {
            it->second->active = isActive;
            dirty = true;
        }
    }

    bool compile(std::vector<std::string>& outErrors);
    
    void execute(const FrameRenderData& frameData,
                 const DebugRenderData& debugData) const;

    const std::vector<std::unique_ptr<RenderPass>>& getPasses() const { return passes; }

private:
    friend class RenderGraphBuilder;
    friend class PassResources;

    bool allocateResources(std::vector<std::string>& outErrors);
    bool isResourceLive(ResourceId id) const;

    struct ResourceNode
    {
        std::string debugName;

        PassId producer = INVALID_PASS_ID;
        std::vector<PassId> readers;

        ResourceDesc desc;

        ResourceId aliasOf = INVALID_RESOURCE_ID;

        void* resource = nullptr;

        bool imported = false;
    };

    ResourceId registerRead  (PassId pass, ResourceId handle);
    ResourceId registerWrite (PassId pass, ResourceId handle);
    ResourceId registerCreate(PassId pass, const std::string& debugName, const ResourceDesc& descs);
    ResourceId registerImport(PassId pass, const std::string& debugName, void* externalResource);

    template<typename T>
    T* resolve(ResourceId handle, PassId requestingPass) const
    {
        auto it = resources.find(handle);
        if (it == resources.end()) return nullptr; // unknown handle

        const ResourceNode& node = it->second;
        bool declaredByPass =
            node.producer == requestingPass ||
            std::find(node.readers.begin(), node.readers.end(), requestingPass) != node.readers.end();

        if (!declaredByPass) return nullptr; 

        return static_cast<T*>(node.resource); 
    }

    
    ResourceDebugInfo toDebugInfo(ResourceId id, const ResourceNode& node) const;

    std::vector<std::unique_ptr<RenderPass>> passes;
    std::unordered_map<PassId, RenderPass*> passIndex;
    std::unordered_map<ResourceId, ResourceNode> resources;
    std::vector<RenderPass*> executionOrder;

    RenderResourceAllocator& m_ResourceAllocator;
    bool dirty = true;
};