#pragma once

#include <cstdint>
#include <vector>
#include <variant>

#include "resources/assets/Texture.h"
#include "render/handlers/RenderId.h"

struct TextureResourceDesc
{
    uint32_t width = 0;
    uint32_t height = 0;

    TextureDesc texture;
};

struct FrameBufferResourceDesc
{
    std::vector<ResourceId> colorAttachments;
    ResourceId depthAttachment = INVALID_RESOURCE_ID;

    
};

using ResourceDesc = std::variant<
    TextureResourceDesc,
    FrameBufferResourceDesc
>;