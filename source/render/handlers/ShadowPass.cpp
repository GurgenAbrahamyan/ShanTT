#include "ShadowPass.h"
#include "scene/SceneRenderData.h"

void ShadowPass::renderStatic(
    const SceneRenderData& scene
)
{
    for (const auto& [material, meshMap] : scene.staticBatches)
    {
        for (const auto& [mesh, batch] : meshMap)
        {
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
                batch.instances.size() * sizeof(Mat4),
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


void ShadowPass::renderSkinned(
    const SceneRenderData& scene
)
{
    if (!m_skinnedShader)
        return;

    m_skinnedShader->Activate();

    m_skinMatrices.bindBase(
        0
    );

    for (const auto& [material, meshMap] : scene.skinnedBatches)
    {
        for (const auto& [mesh, batch] : meshMap)
        {
            if (batch.instances.empty())
                continue;

            const std::size_t count =
                batch.instances.size();

            m_instanceTransforms.resize(count);
            m_instancePaletteOffsets.resize(count);
            m_instanceSkinningEnabled.resize(count);

            for (std::size_t i = 0; i < count; ++i)
            {
                const auto& instance =
                    batch.instances[i];

                m_instanceTransforms[i] =
                    instance.worldTransform;

                m_instancePaletteOffsets[i] =
                    instance.paletteOffset;

                m_instanceSkinningEnabled[i] =
                    instance.skinningEnabled ? 1.0f : 0.0f;
            }

            mesh->bind();

            mesh->setupSkinnedTransformVBO(count);
            mesh->setupSkinnedPaletteOffsetVBO(count);
            mesh->setupSkinnedEnabledVBO(count);

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

            mesh->getSkinnedEnabledVBO()->Bind();

            glBufferSubData(
                GL_ARRAY_BUFFER,
                0,
                count * sizeof(float),
                m_instanceSkinningEnabled.data()
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


void ShadowPass::execute(
    const FrameRenderData& frameData,
    PassResources& resources,
    const DebugRenderData&)
{

    if (!frameData.Has<SceneRenderData>())
        return;

    const auto& sceneData =
        frameData.Get<SceneRenderData>();

    if (sceneData.shadowData.empty())
        return;

    FrameBuffer* framebuffer =
        resources.get<FrameBuffer>(m_shadowFramebuffer);

    if (!framebuffer)
        return;

    // Skin matrices are the same for every shadow-casting light this
    // frame, so upload once, not per-light.
    if (!sceneData.skinMatrices.empty())
    {
        m_skinMatrices.upload(
            sceneData.skinMatrices.data(),
            sceneData.skinMatrices.size() * sizeof(Mat4),
            GL_DYNAMIC_DRAW
        );
    }

    framebuffer->bind();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDisable(GL_CULL_FACE);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 4.0f);

    glClear(GL_DEPTH_BUFFER_BIT);

    constexpr int ATLAS_SIZE =
        ShadowAtlas::ATLAS_SIZE;

    for (const ShadowData& data : sceneData.shadowData)
    {
        const int x =
            static_cast<int>(
                data.uvMin.x * ATLAS_SIZE
            );

        const int y =
            static_cast<int>(
                data.uvMin.y * ATLAS_SIZE
            );

        const int width =
            static_cast<int>(
                (data.uvMax.x - data.uvMin.x) *
                ATLAS_SIZE
            );

        const int height =
            static_cast<int>(
                (data.uvMax.y - data.uvMin.y) *
                ATLAS_SIZE
            );

        glViewport(
            x,
            y,
            width,
            height
        );

        m_shader->Activate();

        m_shader->setMat4(
            "lightSpaceMatrix",
            data.lightMatrix
        );

        renderStatic(sceneData);

        if (m_skinnedShader)
        {
            m_skinnedShader->Activate();

            m_skinnedShader->setMat4(
                "lightSpaceMatrix",
                data.lightMatrix
            );

            renderSkinned(sceneData);
        }
    }

    framebuffer->unbind();

    glDisable(GL_POLYGON_OFFSET_FILL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}