#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

#include "RenderPass.h"
#include "../RenderGraphBuilder.h"
#include "../PassResources.h"

#include "../backend/Shader.h"

#include "../data/FrameRenderData.h"
#include "scene/SceneRenderData.h"

#include "physics/debug_render/DebugRenderData.h"


class DebugRenderPass : public RenderPass
{
public:

    struct DebugRenderPassOptions
    {
        Shader* shader;
    };


    DebugRenderPass(
        RenderGraphBuilder& builder,
        const DebugRenderPassOptions& options
    )
        : RenderPass(
            builder,
            "DebugRender"
        )
        , m_shader(options.shader)
    {
        hasSideEffect = true;

        


        glGenVertexArrays(
            1,
            &m_vao
        );

        glGenBuffers(
            1,
            &m_vbo
        );


        glBindVertexArray(
            m_vao
        );

        glBindBuffer(
            GL_ARRAY_BUFFER,
            m_vbo
        );


        glBufferData(
            GL_ARRAY_BUFFER,
            0,
            nullptr,
            GL_DYNAMIC_DRAW
        );


        glEnableVertexAttribArray(0);

        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(DebugVertex),
            reinterpret_cast<void*>(
                offsetof(
                    DebugVertex,
                    position
                )
            )
        );


        glEnableVertexAttribArray(1);

        glVertexAttribPointer(
            1,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(DebugVertex),
            reinterpret_cast<void*>(
                offsetof(
                    DebugVertex,
                    color
                )
            )
        );


        glBindBuffer(
            GL_ARRAY_BUFFER,
            0
        );

        glBindVertexArray(
            0
        );
    }


    ~DebugRenderPass() override
    {
        if (m_vbo)
        {
            glDeleteBuffers(
                1,
                &m_vbo
            );
        }

        if (m_vao)
        {
            glDeleteVertexArrays(
                1,
                &m_vao
            );
        }
    }


    void execute(
        const FrameRenderData& frameData,
        PassResources&,
        const DebugRenderData&
    ) override
    {
        if (!active)
            return;

        if (!m_shader)
            return;

        if (!frameData.Has<SceneRenderData>())
            return;

        if (!frameData.Has<DebugRenderPhysData>())
            return;


        const auto& scene =
            frameData.Get<SceneRenderData>();

        const auto& physics =
            frameData.Get<DebugRenderPhysData>();


        if (!scene.camera)
            return;


        if (
            physics.lines.empty() &&
            physics.triangles.empty()
        )
        {
            return;
        }


        GLboolean depthEnabled =
            glIsEnabled(GL_DEPTH_TEST);

        GLboolean blendEnabled =
            glIsEnabled(GL_BLEND);

        GLboolean cullEnabled =
            glIsEnabled(GL_CULL_FACE);


        glDisable(GL_DEPTH_TEST);

        glDisable(GL_CULL_FACE);

        glEnable(GL_BLEND);

        glBlendFunc(
            GL_SRC_ALPHA,
            GL_ONE_MINUS_SRC_ALPHA
        );


        m_shader->Activate();


        m_shader->setMat4(
            "view",
            scene.camera->viewMatrix
        );

        m_shader->setMat4(
            "projection",
            scene.camera->projectionMatrix
        );


        glBindVertexArray(
            m_vao
        );


        if (!physics.lines.empty())
        {
            upload(
                physics.lines
            );

            glLineWidth(
                2.0f
            );

            glDrawArrays(
                GL_LINES,
                0,
                static_cast<GLsizei>(
                    physics.lines.size()
                )
            );
        }


        if (!physics.triangles.empty())
        {
            upload(
                physics.triangles
            );

            glDrawArrays(
                GL_TRIANGLES,
                0,
                static_cast<GLsizei>(
                    physics.triangles.size()
                )
            );
        }


        glBindVertexArray(
            0
        );


        if (depthEnabled)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);


        if (blendEnabled)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);


        if (cullEnabled)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
    }


private:

    void upload(
        const std::vector<DebugVertex>& vertices
    )
    {
        glBindBuffer(
            GL_ARRAY_BUFFER,
            m_vbo
        );


        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(
                vertices.size() *
                sizeof(DebugVertex)
            ),
            vertices.data(),
            GL_DYNAMIC_DRAW
        );


        glBindBuffer(
            GL_ARRAY_BUFFER,
            0
        );
    }


private:

    Shader* m_shader = nullptr;

    GLuint m_vao = 0;
    GLuint m_vbo = 0;
};