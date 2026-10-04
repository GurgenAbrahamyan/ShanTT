#pragma once

#include <cstdint>
#include <vector>

#include "RenderPass.h"
#include "../RenderGraphBuilder.h"
#include "../PassResources.h"

#include "scene/SceneRenderData.h"

#include "../backend/Shader.h"
#include "../backend/containers/StorageBuffer.h"

#include "render/ecs_systems/ShadowAtlas.h"

class ShadowPass : public RenderPass
{

public:

    struct ShadowPassOptions{
        Shader* shader;
        Shader* skinnedShader;
    };

    ShadowPass(
        RenderGraphBuilder& builder,
        const ShadowPassOptions& options
    )
        : RenderPass(builder, "Shadow"),
          m_shader(options.shader),
          m_skinnedShader(options.skinnedShader)
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

    void renderStatic(
        const SceneRenderData& scene
    );

    void renderSkinned(
        const SceneRenderData& scene
    );

    Shader* m_shader = nullptr;
    Shader* m_skinnedShader = nullptr;

    StorageBuffer m_skinMatrices;

    std::vector<Mat4> m_instanceTransforms;
    std::vector<uint32_t> m_instancePaletteOffsets;
    std::vector<float> m_instanceSkinningEnabled;

    ResourceId m_shadowTexture =
        INVALID_RESOURCE_ID;

    ResourceId m_shadowFramebuffer =
        INVALID_RESOURCE_ID;

};