#pragma once

#include "RenderPass.h"
#include "../RenderGraphBuilder.h"
#include "../PassResources.h"

#include "../backend/containers/FrameBuffer.h"
#include "../backend/Shader.h"
#include "../backend/containers/VAO.h"
#include "../backend/containers/VBO.h"
#include "resources/assets/Texture.h"

#include <iostream>

class ToneMappingPass : public RenderPass
{
public:

    struct ToneMappingPassSettings
    {
        Shader* shader;

        ResourceId input;

        float exposure = 0.0f;

        uint32_t width;
        uint32_t height;
    };

    ToneMappingPass(
        RenderGraphBuilder& builder,
        const ToneMappingPassSettings& settings
    )
        : RenderPass(builder, "ToneMapping")
        , m_Settings(settings)
        , m_Shader(settings.shader)
        , m_Input(builder.read(settings.input))
        , m_QuadVBO(
            quadVertices,
            sizeof(quadVertices),
            false
        )
    {
        TextureResourceDesc outputDesc;

        outputDesc.width = settings.width;
        outputDesc.height = settings.height;

        outputDesc.texture.internalFormat = GL_RGB16F;
        outputDesc.texture.format = GL_RGBA;
        outputDesc.texture.type = GL_UNSIGNED_BYTE;

        outputDesc.texture.generateMipmaps = false;
        outputDesc.texture.minFilter = GL_LINEAR;
        outputDesc.texture.magFilter = GL_LINEAR;

        m_OutputColor = builder.create(
            "ToneMapping.Color",
            outputDesc
        );

        FrameBufferResourceDesc framebufferDesc;

        framebufferDesc.colorAttachments = {
            m_OutputColor
        };

        m_Output = builder.create(
            "ToneMapping.Framebuffer",
            framebufferDesc
        );

        initializeQuad();
    }

    ResourceId output() const
    {
        return m_OutputColor;
    }

    ResourceId framebuffer() const
    {
        return m_Output;
    }

    void execute(
        const FrameRenderData&,
        PassResources& resources,
        const DebugRenderData&
    ) override
    {
        if (!m_Shader)
            return;

        Texture* input =
            resources.get<Texture>(m_Input);

        FrameBuffer* output =
            resources.get<FrameBuffer>(m_Output);

        if (!input || !output)
            return;


        output->bind();

        glViewport(
            0,
            0,
            output->getWidth(),
            output->getHeight()
        );

        glClear(GL_COLOR_BUFFER_BIT);

        m_Shader->Activate();

        m_Shader->setInt(
            "uBaseInput",
            0
        );

        m_Shader->setFloat(
            "exposure",
            m_Settings.exposure
        );

        input->Bind(0);

        m_QuadVAO.Bind();

        glDrawArrays(
            GL_TRIANGLE_STRIP,
            0,
            4
        );

        m_QuadVAO.Unbind();

        output->unbind();
    }

    ToneMappingPassSettings& getSettings(){
        return m_Settings;
    }

private:

    void initializeQuad()
    {
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

private:

    ToneMappingPassSettings m_Settings;

    Shader* m_Shader = nullptr;

    ResourceId m_Input =
        INVALID_RESOURCE_ID;

    ResourceId m_OutputColor =
        INVALID_RESOURCE_ID;

    ResourceId m_Output =
        INVALID_RESOURCE_ID;


    VAO m_QuadVAO;
    VBO m_QuadVBO;

    static constexpr float quadVertices[16] =
    {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f
    };
};