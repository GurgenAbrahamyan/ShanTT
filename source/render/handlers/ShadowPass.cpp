#include "ShadowPass.h"
#include "scene/SceneRenderData.h"

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

    m_shader->Activate();

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

        m_shader->setMat4(
            "lightSpaceMatrix",
            data.lightMatrix
        );


        for (const auto& [material, meshMap] :
             frameData.Get<SceneRenderData>().staticBatches)
        {
            for (const auto& [mesh, batch] :
                 meshMap)
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
    framebuffer->unbind();

    glDisable(GL_POLYGON_OFFSET_FILL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}