#pragma once

#include <vector>

class Texture;

struct FrameBufferDesc
{
    std::vector<Texture*> colorAttachments;
    Texture* depthAttachment = nullptr;

    bool operator==(const FrameBufferDesc& other) const
    {
        return colorAttachments == other.colorAttachments &&
               depthAttachment == other.depthAttachment;
    }
};