#pragma once

#include <cstdint>
#include <vector>

#include "RenderPass.h"
#include "../RenderGraphBuilder.h"
#include "../PassResources.h"

#include "../backend/containers/FrameBuffer.h"
#include "../backend/Shader.h"
#include "../backend/containers/StorageBuffer.h"

#include "scene/SceneRenderData.h"


class GeometryPass : public RenderPass
{
public:

    struct GeometryPassOptions
    {
        Shader* staticShader;
        Shader* skinnedShader;

        uint32_t width;
        uint32_t height;
    };


    /*
     * GPU representation of one skinned instance.
     *
     * This is deliberately separate from SceneRenderData::SkinnedInstance.
     *
     * SceneRenderData is CPU-side render data.
     * This is exactly the layout we send to the GPU.
     */

    GeometryPass(
        RenderGraphBuilder& builder,
        const GeometryPassOptions& options
    )
        : RenderPass(builder, "Geometry")
        , m_staticShader(options.staticShader)
        , m_skinnedShader(options.skinnedShader)
    {
        /*
         * =========================================================
         * GBUFFER
         * =========================================================
         */

        TextureResourceDesc albedoDesc;

        albedoDesc.width = options.width;
        albedoDesc.height = options.height;

        albedoDesc.texture.internalFormat = GL_RGBA8;
        albedoDesc.texture.format = GL_RGBA;
        albedoDesc.texture.type = GL_UNSIGNED_BYTE;

        albedoDesc.texture.generateMipmaps = false;
        albedoDesc.texture.minFilter = GL_NEAREST;
        albedoDesc.texture.magFilter = GL_NEAREST;

        m_albedo = builder.create(
            "GBuffer.Albedo",
            albedoDesc
        );


        TextureResourceDesc depthDesc;

        depthDesc.width = options.width;
        depthDesc.height = options.height;

        depthDesc.texture.internalFormat = GL_R32F;
        depthDesc.texture.format = GL_RED;
        depthDesc.texture.type = GL_FLOAT;

        depthDesc.texture.generateMipmaps = false;
        depthDesc.texture.minFilter = GL_NEAREST;
        depthDesc.texture.magFilter = GL_NEAREST;

        m_linearDepth = builder.create(
            "GBuffer.LinearDepth",
            depthDesc
        );


        TextureResourceDesc normalDesc;

        normalDesc.width = options.width;
        normalDesc.height = options.height;

        normalDesc.texture.internalFormat = GL_RG16F;
        normalDesc.texture.format = GL_RG;
        normalDesc.texture.type = GL_FLOAT;

        normalDesc.texture.generateMipmaps = false;
        normalDesc.texture.minFilter = GL_NEAREST;
        normalDesc.texture.magFilter = GL_NEAREST;

        m_normal = builder.create(
            "GBuffer.Normal",
            normalDesc
        );


        TextureResourceDesc armDesc;

        armDesc.width = options.width;
        armDesc.height = options.height;

        armDesc.texture.internalFormat = GL_RGBA8;
        armDesc.texture.format = GL_RGBA;
        armDesc.texture.type = GL_UNSIGNED_BYTE;

        armDesc.texture.generateMipmaps = false;
        armDesc.texture.minFilter = GL_NEAREST;
        armDesc.texture.magFilter = GL_NEAREST;

        m_arm = builder.create(
            "GBuffer.ARM",
            armDesc
        );


        TextureResourceDesc emissiveDesc;

        emissiveDesc.width = options.width;
        emissiveDesc.height = options.height;

        emissiveDesc.texture.internalFormat = GL_RGBA16F;
        emissiveDesc.texture.format = GL_RGBA;
        emissiveDesc.texture.type = GL_FLOAT;

        emissiveDesc.texture.generateMipmaps = false;
        emissiveDesc.texture.minFilter = GL_NEAREST;
        emissiveDesc.texture.magFilter = GL_NEAREST;

        m_emissive = builder.create(
            "GBuffer.Emissive",
            emissiveDesc
        );


        TextureResourceDesc hardwareDepthDesc;

        hardwareDepthDesc.width = options.width;
        hardwareDepthDesc.height = options.height;

        hardwareDepthDesc.texture.internalFormat =
            GL_DEPTH_COMPONENT24;

        hardwareDepthDesc.texture.format =
            GL_DEPTH_COMPONENT;

        hardwareDepthDesc.texture.type =
            GL_UNSIGNED_INT;

        hardwareDepthDesc.texture.generateMipmaps =
            false;

        hardwareDepthDesc.texture.minFilter =
            GL_NEAREST;

        hardwareDepthDesc.texture.magFilter =
            GL_NEAREST;

        m_hardwareDepth = builder.create(
            "GBuffer.Depth",
            hardwareDepthDesc
        );


        FrameBufferResourceDesc framebufferDesc;

        framebufferDesc.colorAttachments =
        {
            m_albedo,
            m_linearDepth,
            m_normal,
            m_arm,
            m_emissive
        };

        framebufferDesc.depthAttachment =
            m_hardwareDepth;

        m_framebuffer = builder.create(
            "GBuffer.Framebuffer",
            framebufferDesc
        );
    }


    ResourceId albedo() const
    {
        return m_albedo;
    }

    ResourceId linearDepth() const
    {
        return m_linearDepth;
    }

    ResourceId hardwareDepth() const
    {
        return m_hardwareDepth;
    }

    ResourceId normal() const
    {
        return m_normal;
    }

    ResourceId arm() const
    {
        return m_arm;
    }

    ResourceId emissive() const
    {
        return m_emissive;
    }

    ResourceId framebuffer() const
    {
        return m_framebuffer;
    }


    void execute(
        const FrameRenderData& frameData,
        PassResources& resources,
        const DebugRenderData&
    ) override
    {
        if (!frameData.Has<SceneRenderData>())
            return;


        FrameBuffer* target =
            resources.get<FrameBuffer>(
                m_framebuffer
            );

        if (!target)
            return;


        const auto& scene =
            frameData.Get<SceneRenderData>();

        if (!scene.camera)
            return;


        GLboolean cullWasEnabled =
            glIsEnabled(GL_CULL_FACE);

        glDisable(GL_CULL_FACE);


        target->bind();


        glViewport(
            0,
            0,
            target->getWidth(),
            target->getHeight()
        );


        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );


        renderStatic(
            scene
        );

        renderSkinned(
            scene
        );


        target->unbind();


        if (cullWasEnabled)
        {
            glEnable(GL_CULL_FACE);
        }
    }


private:

    void renderStatic(
        const SceneRenderData& scene
    )
    {
        if (!m_staticShader)
            return;


        m_staticShader->Activate();


        m_staticShader->setMat4(
            "view",
            scene.camera->viewMatrix
        );

        m_staticShader->setMat4(
            "projection",
            scene.camera->projectionMatrix
        );


        for (const auto& [materialId, meshBatches]
             : scene.staticBatches)
        {
            auto materialIt =
                scene.materials.find(materialId);

            if (materialIt ==
                scene.materials.end())
            {
                continue;
            }


            const RenderMaterial& material =
                materialIt->second;

            material.Bind(
                m_staticShader
            );


            for (const auto& [mesh, batch]
                 : meshBatches)
            {
                if (!mesh)
                    continue;

                if (batch.instances.empty())
                    continue;


                mesh->bind();


                mesh->setupInstanceVBO(
                    batch.instances.size()
                );


                
                mesh->getInstanceVBO()->Bind();
                


                glBufferSubData(
                    GL_ARRAY_BUFFER,
                    0,
                    batch.instances.size() *
                        sizeof(Mat4),
                    batch.instances.data()
                );


                glDrawElementsInstanced(
                    GL_TRIANGLES,
                    mesh->indexCount(),
                    GL_UNSIGNED_INT,
                    nullptr,
                    static_cast<GLsizei>(
                        batch.instances.size()
                    )
                );
            }
        }
    }



    void renderSkinned(
    const SceneRenderData& scene
)
{
    if (!m_skinnedShader)
        return;

    if (!scene.skinMatrices.empty())
    {
        m_skinMatrices.upload(
            scene.skinMatrices.data(),
            scene.skinMatrices.size() * sizeof(Mat4),
            GL_DYNAMIC_DRAW
        );
    }

    m_skinMatrices.bindBase(
        0
    );


    m_skinnedShader->Activate();

    m_skinnedShader->setMat4(
        "view",
        scene.camera->viewMatrix
    );

    m_skinnedShader->setMat4(
        "projection",
        scene.camera->projectionMatrix
    );


    for (const auto& [materialId, meshBatches]
         : scene.skinnedBatches)
    {
        auto materialIt =
            scene.materials.find(materialId);

        if (materialIt ==
            scene.materials.end())
        {
            continue;
        }


        const RenderMaterial& material =
            materialIt->second;

        material.Bind(
            m_skinnedShader
        );


        for (const auto& [mesh, batch]
             : meshBatches)
        {
            if (!mesh)
                continue;

            if (batch.instances.empty())
                continue;

            const std::size_t count =
                batch.instances.size();

            m_instanceTransforms.resize(count);
            m_instancePaletteOffsets.resize(count);

            for (std::size_t i = 0; i < count; ++i)
            {
                const auto& instance =
                    batch.instances[i];

                m_instanceTransforms[i] =
                    instance.worldTransform;

                m_instancePaletteOffsets[i] =
                    instance.paletteOffset;
            }


            mesh->bind();

            mesh->setupSkinnedTransformVBO(count);
            mesh->setupSkinnedPaletteOffsetVBO(count);


            mesh->getSkinnedTransformVBO()->Bind();

            glBufferSubData(
                GL_ARRAY_BUFFER,
                0,
                count * sizeof(Mat4),
                m_instanceTransforms.data()
            );
            
            mesh->getSkinnedPaletteOffsetVBO()->Bind();

            glBufferSubData(
                GL_ARRAY_BUFFER,
                0,
                count * sizeof(uint32_t),
                m_instancePaletteOffsets.data()
            );


            glDrawElementsInstanced(
                GL_TRIANGLES,
                mesh->indexCount(),
                GL_UNSIGNED_INT,
                nullptr,
                static_cast<GLsizei>(count)
            );
        }
    }
}

private:

    Shader* m_staticShader = nullptr;
    Shader* m_skinnedShader = nullptr;

    StorageBuffer m_skinMatrices;

    // Buffer A: per-instance world transforms
    std::vector<Mat4> m_instanceTransforms;

    // Buffer B: per-instance palette offsets
    std::vector<uint32_t> m_instancePaletteOffsets;

    ResourceId m_albedo =
        INVALID_RESOURCE_ID;

    ResourceId m_linearDepth =
        INVALID_RESOURCE_ID;

    ResourceId m_normal =
        INVALID_RESOURCE_ID;

    ResourceId m_arm =
        INVALID_RESOURCE_ID;

    ResourceId m_emissive =
        INVALID_RESOURCE_ID;

    ResourceId m_hardwareDepth =
        INVALID_RESOURCE_ID;

    ResourceId m_framebuffer =
        INVALID_RESOURCE_ID;
};