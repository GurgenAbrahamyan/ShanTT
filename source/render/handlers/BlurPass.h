#pragma once

#include "RenderPass.h"
#include "../RenderGraphBuilder.h"
#include "../PassResources.h"

#include "render/backend/Shader.h"
#include "render/backend/containers/VAO.h"
#include "render/backend/containers/VBO.h"

#include "ecs/components/graphics/CameraComponent.h"
#include "scene/SceneRenderData.h"

#include <glad/glad.h>

class BlurPass : public RenderPass
{
    
public:

    struct BlurPassSettings{
        Shader* shader;
        ResourceId input;
        ResourceId inputFramebuffer; // needed for pass-through blit when blur is off
        ResourceId hardwareDepth;
        uint32_t width;
        uint32_t height;
    };

    BlurPass(
        RenderGraphBuilder& builder,
        const BlurPassSettings& settings
    )
        : RenderPass(builder, "Blur")
        , m_Shader(settings.shader)
        , m_hardwareDepth(builder.read(settings.hardwareDepth))
        , m_Input(builder.read(settings.input))
        , m_InputFramebuffer(builder.read(settings.inputFramebuffer))
        , m_QuadVBO(
            quadVertices,
            sizeof(quadVertices),
            false
        )
    {
        m_Output_Color = builder.create("BlurPass.Color", 
            {TextureResourceDesc{
                settings.width, 
                settings.height, 
                { .internalFormat = GL_RGBA16F,
                  .wrapS = GL_CLAMP_TO_EDGE,
                  .wrapT = GL_CLAMP_TO_EDGE}
            }});
        m_Output = builder.create("BlurPass.Buffer", 
                        FrameBufferResourceDesc{{m_Output_Color} } );
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
        const FrameRenderData& frameData,
        PassResources& resources,
        const DebugRenderData&
    ) override
    {
        if (!m_Shader)
            return;

        if (m_Input == INVALID_RESOURCE_ID ||
            m_Output == INVALID_RESOURCE_ID)
        {
            return;
        }

        const auto& sceneData = frameData.Get<SceneRenderData>();

        if (!sceneData.camera)
            return;

        const CameraComponent& camera =
            *sceneData.camera;

        FrameBuffer* output =
            resources.get<FrameBuffer>(m_Output);

        if (!output)
            return;

        if (!camera.applyBlur)
        {
            FrameBuffer* inputFb =
                resources.get<FrameBuffer>(m_InputFramebuffer);

            if (!inputFb)
                return;

            glBindFramebuffer(GL_READ_FRAMEBUFFER, inputFb->getID());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, output->getID());
            glBlitFramebuffer(
                0, 0, inputFb->getWidth(), inputFb->getHeight(),
                0, 0, output->getWidth(), output->getHeight(),
                GL_COLOR_BUFFER_BIT, GL_NEAREST
            );

            return;
        }

        Texture* input =
            resources.get<Texture>(m_Input);

        Texture* depth =
            resources.get<Texture>(m_hardwareDepth);

        if (!input)
            return;

        output->bind();

        glViewport(
            0,
            0,
            output->getWidth(),
            output->getHeight()
        );

        glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT);

        m_Shader->Activate();

        m_Shader->setInt(
            "uSceneTexture",
            0
        );

        m_Shader->setInt(
            "uDepthTexture",
            1
        );

        m_Shader->setFloat(
            "uFocusDistance",
            camera.focusDistance
        );

        m_Shader->setFloat(
            "uAperture",
            camera.aperture
        );

        m_Shader->setFloat(
            "uFocalLength",
            camera.focalLength
        );

        m_Shader->setFloat(
            "uBlurScale",
            camera.blurScale
        );

        m_Shader->setFloat(
            "uNearPlane",
            camera.nearPlane
        );

        m_Shader->setFloat(
            "uFarPlane",
            camera.farPlane
        );

        input->Bind(0);

        if (depth)
            depth->Bind(1);

        m_QuadVAO.Bind();

        glDrawArrays(
            GL_TRIANGLE_STRIP,
            0,
            4
        );

        m_QuadVAO.Unbind();

        output->unbind();
    }

    ResourceId output(){
        return m_Output_Color;
    }

private:
    Shader* m_Shader = nullptr;
    ResourceId m_hardwareDepth =
        INVALID_RESOURCE_ID;

    ResourceId m_Input =
        INVALID_RESOURCE_ID;

    ResourceId m_InputFramebuffer =
        INVALID_RESOURCE_ID;

    ResourceId m_Output =
        INVALID_RESOURCE_ID;
    
    ResourceId m_Output_Color =
        INVALID_RESOURCE_ID;

    VAO m_QuadVAO;
    VBO m_QuadVBO;

    static constexpr float quadVertices[16] =
    {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f
    };
};