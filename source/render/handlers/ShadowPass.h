#pragma once

#include "RenderPass.h"
#include "../RenderGraphBuilder.h"
#include "../PassResources.h"

#include "../backend/Shader.h"

#include "render/ecs_systems/ShadowAtlas.h"

class ShadowPass : public RenderPass
{
   
public:

    struct ShadowPassOptions{
        Shader* shader;
    };

    ShadowPass(
        RenderGraphBuilder& builder,
        const ShadowPassOptions& options
    )
        : RenderPass(builder, "Shadow"),
          m_shader(options.shader)
    {
        TextureDesc shadowTextureDesc;

        shadowTextureDesc.target = GL_TEXTURE_2D;
        shadowTextureDesc.internalFormat = GL_DEPTH_COMPONENT32F;
        shadowTextureDesc.format = GL_DEPTH_COMPONENT;
        shadowTextureDesc.type = GL_FLOAT;

        shadowTextureDesc.generateMipmaps = false;
        shadowTextureDesc.minFilter = GL_NEAREST;
        shadowTextureDesc.magFilter = GL_NEAREST;

        shadowTextureDesc.wrapS = GL_CLAMP_TO_EDGE;
        shadowTextureDesc.wrapT = GL_CLAMP_TO_EDGE;

        TextureResourceDesc textureDesc;

        textureDesc.width = ShadowAtlas::ATLAS_SIZE;
        textureDesc.height = ShadowAtlas::ATLAS_SIZE;
        textureDesc.texture = shadowTextureDesc;

        m_shadowTexture =
            builder.create(
                "ShadowAtlas",
                textureDesc
            );

        FrameBufferResourceDesc framebufferDesc;

        framebufferDesc.depthAttachment =
            m_shadowTexture;

        m_shadowFramebuffer =
            builder.create(
                "ShadowFramebuffer",
                framebufferDesc
            );


        hasSideEffect = true;
    }

    ResourceId shadowTexture() const
    {
        return m_shadowTexture;
    }

    ResourceId shadowFramebuffer() const
    {
        return m_shadowFramebuffer;
    }

    void execute(
        const FrameRenderData& frameData,
        PassResources& resources,
        const DebugRenderData&
    ) override;

private:
    Shader* m_shader = nullptr;

    ResourceId m_shadowTexture =
        INVALID_RESOURCE_ID;

    ResourceId m_shadowFramebuffer =
        INVALID_RESOURCE_ID;

};