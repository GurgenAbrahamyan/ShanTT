#pragma once

#include "RenderPass.h"
#include "../RenderGraphBuilder.h"
#include "../PassResources.h"

#include "render/backend/Shader.h"
#include "render/backend/containers/VAO.h"
#include "render/backend/containers/VBO.h"

#include <glad/glad.h>

#include <cstdint>
#include <vector>

#include "math_custom/Vector2.h"

class BloomPass : public RenderPass
{

    

public:
    struct BloomPassSettings
    {
        Shader* downScaleShader;
        Shader* upScaleShader;
        ResourceId input;
        uint32_t width;
        uint32_t height;
        int mipMapLength = 5;
        float filterRadius = 0.005f;
    };

    BloomPass(
        RenderGraphBuilder& builder,
        const BloomPassSettings& settings
    )
        : RenderPass(builder, "Bloom")
        , m_DownsampleShader(settings.downScaleShader)
        , m_UpsampleShader(settings.upScaleShader)
        , m_Width(settings.width)
        , m_Height(settings.height)
        , m_Settings(settings)
        , m_Input(builder.read(settings.input))
        , m_Output()
        , m_QuadVBO(
            quadVertices,
            sizeof(quadVertices),
            false
        )
    {
        if (!m_DownsampleShader || !m_UpsampleShader)
            return;

        createMipResources(builder);
       
        m_Output = builder.create("BloomPass.FrameBuffer", 
                                FrameBufferResourceDesc{
                                    {m_MipLevels[0].resource}
                                });

        initializeQuad();
    }

    void execute(
        const FrameRenderData&,
        PassResources& resources,
        const DebugRenderData&
    ) override
    {
        if (m_Input == INVALID_RESOURCE_ID ||
            m_Output == INVALID_RESOURCE_ID)
        {
            return;
        }

        if (!m_DownsampleShader ||
            !m_UpsampleShader ||
            m_MipLevels.empty())
        {
            return;
        }

        downsample(resources);
        upsample(resources);
    }

    BloomPassSettings& getSettings() { return m_Settings;}

    ResourceId output(){
        return m_MipLevels[0].resource;
    }

private:
    struct MipLevel
    {
        ResourceId resource = INVALID_RESOURCE_ID;
        uint32_t width = 0;
        uint32_t height = 0;
    };

    Shader* m_DownsampleShader = nullptr;
    Shader* m_UpsampleShader = nullptr;

    uint32_t m_Width = 0;
    uint32_t m_Height = 0;

    BloomPassSettings m_Settings;

    ResourceId m_Input = INVALID_RESOURCE_ID;
    ResourceId m_Output = INVALID_RESOURCE_ID;


    std::vector<MipLevel> m_MipLevels;

    VAO m_QuadVAO;
    VBO m_QuadVBO;

private:
    static constexpr float quadVertices[16] =
    {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f
    };

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

    void createMipResources(RenderGraphBuilder& builder)
    {
        m_MipLevels.clear();

        if (m_Settings.mipMapLength <= 0)
            return;

        uint32_t width = m_Width;
        uint32_t height = m_Height;

        for (int i = 0;
             i < m_Settings.mipMapLength;
             ++i)
        {
            width = std::max(1u, width);
            height = std::max(1u, height);

            TextureDesc desc;

            desc.internalFormat = GL_R11F_G11F_B10F;
            desc.format = GL_RGB;
            desc.type = GL_FLOAT;

            desc.generateMipmaps = false;

            desc.minFilter = GL_LINEAR;
            desc.magFilter = GL_LINEAR;

            desc.wrapS = GL_CLAMP_TO_EDGE;
            desc.wrapT = GL_CLAMP_TO_EDGE;

            ResourceId mip =
                builder.create(
                    "Bloom.Mip" + std::to_string(i),
                    TextureResourceDesc{
                        width,
                        height,
                        desc
                    }
                );

            m_MipLevels.push_back({
                mip,
                width,
                height
            });

            width = std::max(1u, width / 2);
            height = std::max(1u, height / 2);
        }
    }

    void downsample(PassResources& resources)
    {
        Texture* inputTexture =
            resources.get<Texture>(m_Input);

        if (!inputTexture)
            return;

        m_DownsampleShader->Activate();

        m_DownsampleShader->setVec2(
            "srcResolution",
            { static_cast<float>(m_Width),
              static_cast<float>(m_Height)
            }
        );

        inputTexture->Bind(0);

        m_QuadVAO.Bind();

        for (size_t i = 0;
             i < m_MipLevels.size();
             ++i)
        {
            const MipLevel& mip = m_MipLevels[i];

            Texture* target =
                resources.get<Texture>(mip.resource);

            if (!target)
                continue;

            glViewport(
                0,
                0,
                static_cast<GLsizei>(mip.width),
                static_cast<GLsizei>(mip.height)
            );

            FrameBuffer* framebuffer =
                resources.get<FrameBuffer>(m_Output);

            if (!framebuffer)
                continue;

            framebuffer->attachColor(
                target->getID(),
                0
            );

            framebuffer->bind();

            glDrawArrays(
                GL_TRIANGLE_STRIP,
                0,
                4
            );

            framebuffer->unbind();

            m_DownsampleShader->setVec2(
                "srcResolution",
                {
                    static_cast<float>(mip.width),
                    static_cast<float>(mip.height)
                }
            );

            target->Bind(0);
        }
        
        m_QuadVAO.Unbind();
    }

    void upsample(PassResources& resources)
    {
        FrameBuffer* output =
            resources.get<FrameBuffer>(m_Output);

        if (!output)
            return;

        m_UpsampleShader->Activate();

        m_UpsampleShader->setFloat(
            "filterRadius",
            m_Settings.filterRadius
        );

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);

        m_QuadVAO.Bind();

        for (int i =
                 static_cast<int>(m_MipLevels.size()) - 1;
             i > 0;
             --i)
        {
            const MipLevel& current =
                m_MipLevels[i];

            const MipLevel& next =
                m_MipLevels[i - 1];

            Texture* currentTexture =
                resources.get<Texture>(current.resource);

            Texture* nextTexture =
                resources.get<Texture>(next.resource);

            if (!currentTexture ||
                !nextTexture)
            {
                continue;
            }

            currentTexture->Bind(0);

            output->attachColor(
                nextTexture->getID(),
                0
            );

            output->bind();

            glViewport(
                0,
                0,
                static_cast<GLsizei>(next.width),
                static_cast<GLsizei>(next.height)
            );

            glDrawArrays(
                GL_TRIANGLE_STRIP,
                0,
                4
            );

            output->unbind();
        }

        m_QuadVAO.Unbind();

        glDisable(GL_BLEND);
    }
};