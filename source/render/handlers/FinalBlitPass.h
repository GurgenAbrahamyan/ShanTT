#pragma once

#include "RenderPass.h"
#include "../RenderGraphBuilder.h"
#include "../PassResources.h"

#include "../backend/Shader.h"
#include "render/data/FrameRenderData.h"

class FinalBlitPass : public RenderPass
{
public:

    struct FinalBlitPassSettings
    {
        Shader* shader;
        ResourceId input;

        uint32_t width;
        uint32_t height;

        bool gammaCorrect = true;
    };

    FinalBlitPass(
        RenderGraphBuilder& builder,
        const FinalBlitPassSettings& settings
    )
        : RenderPass(builder, "FinalBlit")
        , m_Shader(settings.shader)
        , m_Input(builder.read(settings.input))
        , m_Width(settings.width)
        , m_Height(settings.height)
        , m_GammaCorrect(settings.gammaCorrect)
        , m_QuadVBO(
            quadVertices,
            sizeof(quadVertices),
            false)
    {
        hasSideEffect = true;

        m_QuadVAO.Bind();
        m_QuadVBO.Bind();

        m_QuadVAO.LinkAttrib(
            m_QuadVBO,
            0,
            2,
            GL_FLOAT,
            4 * sizeof(float),
            (void*)0
        );

        m_QuadVAO.LinkAttrib(
            m_QuadVBO,
            1,
            2,
            GL_FLOAT,
            4 * sizeof(float),
            (void*)(2 * sizeof(float))
        );

        m_QuadVAO.Unbind();
        m_QuadVBO.Unbind();
    }

    void execute(
        const FrameRenderData&,
        PassResources& resources,
        const DebugRenderData&
    ) override
    {
        if (!m_Shader)
            return;

        Texture* inputTexture =
            resources.get<Texture>(m_Input);

        if (!inputTexture)
            return;

         
        glBindFramebuffer(
            GL_FRAMEBUFFER,
            0
        );

        glViewport(
            0,
            0,
            m_Width,
            m_Height
        );

        glDisable(GL_DEPTH_TEST);

        m_Shader->Activate();

        m_Shader->setInt(
            "sceneTexture",
            0
        );

        m_Shader->setInt(
            "gammaCorrect",
            m_GammaCorrect
        );

        inputTexture->Bind(0);

        m_QuadVAO.Bind();

        glDrawArrays(
            GL_TRIANGLE_STRIP,
            0,
            4
        );

        m_QuadVAO.Unbind();

        glBindTexture(
            GL_TEXTURE_2D,
            0
        );

        glEnable(GL_DEPTH_TEST);
    }

private:

    Shader* m_Shader = nullptr;

    ResourceId m_Input =
        INVALID_RESOURCE_ID;

    uint32_t m_Width = 0;
    uint32_t m_Height = 0;

    bool m_GammaCorrect = true;

    VAO m_QuadVAO;
    VBO m_QuadVBO;

    static constexpr float quadVertices[16] =
    {
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f
    };
};