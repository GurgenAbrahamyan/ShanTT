#pragma once
#include "backend/containers/FrameBuffer.h"
#include "handlers/RenderId.h"

class RenderResourcePool
{
public:
    void registerTarget(ResourceId id, FramebufferDesc desc); // logical decl
    void createOrResize(int width, int height);                // actual GL calls, once + on resize
    Framebuffer* get(ResourceId id) const;                      // passes call this in execute()

private:
    std::unordered_map<ResourceId, std::unique_ptr<Framebuffer>> targets;
};