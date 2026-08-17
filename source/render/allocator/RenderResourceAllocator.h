#pragma once

#include "resources/assets/Texture.h"
#include "render/backend/containers/FrameBuffer.h"
#include "render/backend/containers/FrameBufferDesc.h"

#include <memory>
#include <vector>

class RenderResourceAllocator
{
public:
    Texture* acquireRenderTarget(uint32_t width, uint32_t height, const TextureDesc& desc);

    FrameBuffer* acquireFramebuffer(
        const FrameBufferDesc& desc
    );

    void releaseRenderTarget(Texture* texture);
    void releaseFramebuffer(FrameBuffer* framebuffer);

    void releaseAll();

private:
    struct RenderTargetEntry
    {
        std::unique_ptr<Texture> resource;
        uint32_t width, height;
        TextureDesc desc;
        bool inUse = false;
    };

    struct FramebufferEntry
    {
        std::unique_ptr<FrameBuffer> resource;
        FrameBufferDesc desc;
        bool inUse = false;
    };

    std::vector<RenderTargetEntry> m_RenderTargets;
    std::vector<FramebufferEntry> m_Framebuffers;
};