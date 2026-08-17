#pragma once

#include "RenderPass.h"
#include "../RenderGraphBuilder.h"
#include "../PassResources.h"

#include "../backend/containers/FrameBuffer.h"
#include "../backend/Shader.h"
#include "resources/assets/Texture.h"

#include "scene/EnvironmentRenderData.h"
#include "render/data/FrameRenderData.h"
#include "scene/SceneRenderData.h"

#include "resources/assets/Texture.h"

static constexpr float kCubeVertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

class CubeMapPass : public RenderPass
{
public:

    struct CubeMapPassSettings
    {
        Shader* shader;
        ResourceId lightingColor; 
        ResourceId hardwareDepth;
    };

    CubeMapPass(
        RenderGraphBuilder& builder,
        const CubeMapPassSettings& settings
    )
        : RenderPass(builder, "CubeMap")
        , m_CubeVAO()
        , m_CubeVBO(kCubeVertices, sizeof(kCubeVertices), false)
        , m_Shader(settings.shader)
    {
        colorRW = builder.write(settings.lightingColor);
        ResourceId depthRO = builder.read(settings.hardwareDepth);

        FrameBufferResourceDesc framebufferDesc;
        framebufferDesc.colorAttachments = { colorRW };
        framebufferDesc.depthAttachment  = depthRO;

        m_Output = builder.create(
            "CubeMapPass.Framebuffer",
            framebufferDesc
        );

        hasSideEffect = true;

        genCubeMesh();

        hasSideEffect = true;
    }

    ResourceId framebuffer() const
    {
        return m_Output;
    }

    ResourceId output() const
    {
        return colorRW;
    }

    void execute(
        const FrameRenderData& frameData,
        PassResources& resources,
        const DebugRenderData&
    ) override
    {
        if (!m_Shader)
            return;

        if (!frameData.Has<SceneRenderData>())
            return;

        if (!frameData.Has<EnvironmentRenderData>())
            return;

        const auto& scene = frameData.Get<SceneRenderData>();
        const auto& environment = frameData.Get<EnvironmentRenderData>();

        if (!scene.camera)
            return;

        if (!environment.env)
            return;

        FrameBuffer* target = resources.get<FrameBuffer>(m_Output);
        Texture* targetTexture = resources.get<Texture>(colorRW);

        if (!target)
            return;

        target->bind();
        glViewport(0, 0, targetTexture->getWidth(), targetTexture->getHeight());

        BindCubeMesh();
        m_Shader->Activate();

        environment.env->bind(0);
        m_Shader->setInt("skybox", 0);

        m_Shader->setMat4("projection", scene.camera->projectionMatrix);
        m_Shader->setMat4("view", scene.camera->viewMatrix);

        m_Shader->setFloat(
            "envIntensity",
            environment.intensity * environment.dirLightInfluence
        );


        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        target->unbind();
    }

private:

    void genCubeMesh()
    {
        m_CubeVAO.Bind();
        m_CubeVAO.LinkAttrib(m_CubeVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
        m_CubeVAO.Unbind();
    }

    void BindCubeMesh()
    {
        m_CubeVAO.Bind();
    }

    VAO m_CubeVAO;
    VBO m_CubeVBO;

    Shader* m_Shader = nullptr;

    ResourceId m_Output = INVALID_RESOURCE_ID;
    
    ResourceId colorRW = INVALID_RESOURCE_ID;
};