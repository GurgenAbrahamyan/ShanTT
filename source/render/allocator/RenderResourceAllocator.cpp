#include "RenderResourceAllocator.h"

#include <cassert>
#include <iostream>

Texture* RenderResourceAllocator::acquireRenderTarget(
    uint32_t width,
    uint32_t height,
    const TextureDesc& desc)
{
    std::cout
        << "\n[Allocator] acquireRenderTarget"
        << " requested: "
        << width << "x" << height
        << " existing: " << m_RenderTargets.size()
        << "\n";

    for (size_t i = 0; i < m_RenderTargets.size(); ++i)
    {
        auto& entry = m_RenderTargets[i];

        std::cout
            << "  [Candidate " << i << "]"
            << " texture=" << entry.resource.get()
            << " GL_ID=" << entry.resource->getID()
            << " size=" << entry.width << "x" << entry.height
            << " inUse=" << entry.inUse
            << "\n";

        if (entry.inUse)
            continue;

        std::cout
            << "  -> REUSING texture "
            << entry.resource->getID()
            << "\n";

        entry.inUse = true;
        return entry.resource.get();
    }

    auto texture = std::make_unique<Texture>(
        width,
        height,
        nullptr,
        desc
    );

    auto* result = texture.get();

    std::cout
        << "  -> CREATING NEW texture"
        << " ptr=" << result
        << " GL_ID=" << result->getID()
        << " size=" << result->getWidth()
        << "x" << result->getHeight()
        << "\n";

    m_RenderTargets.push_back({
        std::move(texture),
        width,
        height,
        desc,
        true
    });

    return result;
}

FrameBuffer* RenderResourceAllocator::acquireFramebuffer(
    const FrameBufferDesc& desc)
{
    
    for (auto& entry : m_Framebuffers)
    {
        if (entry.inUse)
            continue;

        if (!(entry.desc == desc))
            continue;

        entry.inUse = true;
        return entry.resource.get();
    }

    
    uint32_t width = 0;
    uint32_t height = 0;

    if (!desc.colorAttachments.empty() &&
        desc.colorAttachments.front())
    {
        width =
            static_cast<uint32_t>(
                desc.colorAttachments.front()->getWidth()
            );

        height =
            static_cast<uint32_t>(
                desc.colorAttachments.front()->getHeight()
            );
    }
    else if (desc.depthAttachment)
    {
        width =
            static_cast<uint32_t>(
                desc.depthAttachment->getWidth()
            );

        height =
            static_cast<uint32_t>(
                desc.depthAttachment->getHeight()
            );
    }

    auto framebuffer =
        std::make_unique<FrameBuffer>(width, height);

    for (size_t i = 0;
         i < desc.colorAttachments.size();
         ++i)
    {
        Texture* texture = desc.colorAttachments[i];

        if (!texture)
            continue;

        framebuffer->attachColor(
            texture->getID(),
            i
        );
    }

    if (desc.depthAttachment)
    {
        framebuffer->attachDepth(
            desc.depthAttachment->getID()
        );
    }

    assert(
        framebuffer->isComplete() &&
        "RenderResourceAllocator: incomplete framebuffer"
    );


auto* result = framebuffer.get();


m_Framebuffers.push_back({
    std::move(framebuffer),
    desc,
    true
});
    return result;
}

void RenderResourceAllocator::releaseRenderTarget(
    Texture* texture)
{
    if (!texture)
        return;

    for (auto& entry : m_RenderTargets)
    {
        if (entry.resource.get() == texture)
        {
            entry.inUse = false;
            return;
        }
    }

    assert(
        false &&
        "RenderResourceAllocator: attempted to release unknown render target"
    );
}

void RenderResourceAllocator::releaseFramebuffer(FrameBuffer* framebuffer)
{
    if (!framebuffer)
        return;

    for (auto& entry : m_Framebuffers)
    {
        if (entry.resource.get() == framebuffer)
        {
            entry.inUse = false;
            return;
        }
    }

    assert(
        false &&
        "RenderResourceAllocator: attempted to release unknown framebuffer"
    );
}

void RenderResourceAllocator::releaseAll()
{
    m_Framebuffers.clear();
    m_RenderTargets.clear();
}