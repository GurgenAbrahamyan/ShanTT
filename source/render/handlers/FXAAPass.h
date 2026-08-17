#pragma once

#include "RenderPass.h"
#include "../RenderGraphBuilder.h"
#include "../PassResources.h"

#include "../backend/containers/FrameBuffer.h"
#include "../backend/Shader.h"

class FXAAPass : public RenderPass
{
    
public:
    struct FXAAPassSettings
    {
        Shader* shader;
        ResourceId input;
        uint32_t width;
        uint32_t height;
        float edgeThreshold = 0.2f;
        float blendStrength = 0.4f;
    };

    FXAAPass(
        RenderGraphBuilder& builder,
        const FXAAPassSettings& options
        )
        : RenderPass(builder, "FXAA")
        , settings(options)
        , m_Shader(options.shader)
        , m_Input(builder.read(options.input))
        , m_QuadVBO(
            quadVertices,
            sizeof(quadVertices),
            false)
    {

        m_OutputColor =
            builder.create(
                "FXAA.Color",
                TextureResourceDesc{
                    options.width,
                    options.height,
                    {
                        .internalFormat = GL_RGBA16F,
                        .wrapS = GL_CLAMP_TO_EDGE,
                        .wrapT = GL_CLAMP_TO_EDGE
                    }
                }
            );

        m_Output =
            builder.create(
                "FXAA.Buffer",
                FrameBufferResourceDesc{
                    { m_OutputColor }
                }
            );

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

    ResourceId output() const
    {
        return m_OutputColor;
    }

    void execute(
        const FrameRenderData&,
        PassResources& resources,
        const DebugRenderData&) override
    {
        if (!m_Shader)
            return;

        Texture* input =
            resources.get<Texture>(m_Input);

        FrameBuffer* outputFB =
            resources.get<FrameBuffer>(m_Output);

        if (!input || !outputFB)
            return;

        outputFB->bind();

        glViewport(
            0,
            0,
            outputFB->getWidth(),
            outputFB->getHeight()
        );

        glDisable(GL_DEPTH_TEST);

        m_Shader->Activate();

        m_Shader->setInt(
            "screenTex",
            0
        );

        m_Shader->setFloat(
            "threshold",
            settings.edgeThreshold
        );

        m_Shader->setFloat(
            "blendStrength",
            settings.blendStrength
        );

        input->Bind(0);

        m_QuadVAO.Bind();

        glDrawArrays(
            GL_TRIANGLE_STRIP,
            0,
            4
        );

        m_QuadVAO.Unbind();

        outputFB->unbind();

        glEnable(GL_DEPTH_TEST);
    }

    FXAAPassSettings& getSettings(){
        return settings;
    }
    

private:

    FXAAPassSettings settings;

    Shader* m_Shader = nullptr;

    ResourceId m_Input =
        INVALID_RESOURCE_ID;

    ResourceId m_Output =
        INVALID_RESOURCE_ID;

    ResourceId m_OutputColor =
        INVALID_RESOURCE_ID;

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