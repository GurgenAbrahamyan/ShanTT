#pragma once

#include "RenderPass.h"
#include "../RenderGraphBuilder.h"
#include "../PassResources.h"

#include "../data/FrameRenderData.h"
#include "scene/SceneRenderData.h"
#include "scene/EnvironmentRenderData.h"
#include "scene/GlobalTexturesRenderData.h"

#include "../backend/Shader.h"
#include "../backend/containers/GlobalUniformBuffer.h"

class LightingPass : public RenderPass
{
    
public:

    struct LightingPassOptions{
        Shader* shader;
        ResourceId gAlbedo   = INVALID_RESOURCE_ID;
        ResourceId gLinDepth = INVALID_RESOURCE_ID;
        ResourceId gNormal   = INVALID_RESOURCE_ID;
        ResourceId gARM      = INVALID_RESOURCE_ID;
        ResourceId gEmissive = INVALID_RESOURCE_ID;        
        ResourceId shadowMap = INVALID_RESOURCE_ID;
        uint32_t width; 
        uint32_t height;
    };

    LightingPass(
        RenderGraphBuilder& builder,
        const LightingPassOptions& options
    )
        : RenderPass(builder, "Lighting")
        , m_Shader(options.shader)
        , m_GAlbedo(builder.read(options.gAlbedo))
        , m_GLinDepth(builder.read(options.gLinDepth))
        , m_GNormal(builder.read(options.gNormal))
        , m_GARM(builder.read(options.gARM))
        , m_GEmissive(builder.read(options.gEmissive))
        , m_ShadowMap(
            options.shadowMap != INVALID_RESOURCE_ID
                ? builder.read(options.shadowMap)
                : INVALID_RESOURCE_ID)
        , m_QuadVBO(quadVertices, sizeof(quadVertices), false)
    {
        TextureResourceDesc desc {};
        desc.width = options.width;
        desc.height = options.height;
        desc.texture.internalFormat = GL_RGBA16F;
        desc.texture.format = GL_RGBA;
        desc.texture.type = GL_UNSIGNED_BYTE;
        desc.texture.generateMipmaps = false;
        desc.texture.minFilter = GL_NEAREST;
        desc.texture.magFilter = GL_NEAREST;

        m_Output = builder.create("Lighting.HDRcolor", desc);

        FrameBufferResourceDesc framebufferDesc;

        framebufferDesc.colorAttachments = {
            m_Output
        };

        m_FrameBuffer = builder.create(
            "Lighting.Framebuffer",
            framebufferDesc
        );

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

        hasSideEffect = true;
    }

    ResourceId output() const
    {
        return m_Output;
    }

    void execute(
        const FrameRenderData& frameData,
        PassResources& resources,
        const DebugRenderData&) override
    {
        if (!m_Shader)
            return;

        if (!frameData.Has<SceneRenderData>())
            return;

        const auto& scene =
            frameData.Get<SceneRenderData>();

        if (!scene.camera ||
            !scene.cameraTransform)
        {
            return;
        }

        FrameBuffer* output =
            resources.get<FrameBuffer>(m_FrameBuffer);
          
        Texture* gAlbedo =
            resources.get<Texture>(m_GAlbedo);
          
        Texture* gLinDepth =
            resources.get<Texture>(m_GLinDepth);
          
        Texture* gNormal =
            resources.get<Texture>(m_GNormal);
            
        Texture* gARM =
            resources.get<Texture>(m_GARM);
            
        Texture* gEmissive =
            resources.get<Texture>(m_GEmissive);
           
        if (!output || !gAlbedo || !gLinDepth ||
            !gNormal|| !gARM    || !gEmissive)
            return;


        m_Shader->Activate();

        output->bind();

        glViewport(
            0,
            0,
            output->getWidth(),
            output->getHeight()
        );

        const CameraComponent& camera =
            *scene.camera;

        m_Shader->setVec3(
            "cameraPos",
            scene.cameraTransform->position
        );

        m_Shader->setMat4(
            "invView",
            camera.viewMatrix.inverse()
        );

        m_Shader->setMat4(
            "invProjection",
            camera.projectionMatrix.inverse()
        );

        gAlbedo->Bind(0);

        gLinDepth->Bind(1);

        gNormal->Bind(2);

        gARM->Bind(3);

        gEmissive->Bind(4);

        m_Shader->setInt("gAlbedo", 0);
        m_Shader->setInt("gDepth", 1);
        m_Shader->setInt("gNormal", 2);
        m_Shader->setInt("gARM", 3);
        m_Shader->setInt("gEmissive", 4);


        if (frameData.Has<EnvironmentRenderData>())
        {
            const auto& environment =
                frameData.Get<EnvironmentRenderData>();

            if (environment.irr)
            {
                glActiveTexture(GL_TEXTURE10);
                environment.irr->bind(10);

                m_Shader->setInt(
                    "irradianceMap",
                    10
                );
            }

            if (environment.prefilter)
            {
                glActiveTexture(GL_TEXTURE11);
                environment.prefilter->bind(11);

                m_Shader->setInt(
                    "prefilterMap",
                    11
                );
            }

            if (frameData.Has<GlobalTextureRenderData>())
            {
                const auto& globalTextures =
                    frameData.Get<GlobalTextureRenderData>();

                if (globalTextures.brdfLUT)
                {
                    glActiveTexture(GL_TEXTURE12);
                    globalTextures.brdfLUT->Bind(12);

                    m_Shader->setInt(
                        "brdfLUT",
                        12
                    );
                }
            }

            m_Shader->setFloat(
                "envIntensity",
                environment.intensity * environment.dirLightInfluence
            );
        }

        uploadLights(scene);
        uploadLightMatrices(scene);

        bool shadowsEnabled = false;

        if (m_ShadowMap != INVALID_RESOURCE_ID &&
            frameData.Has<SceneRenderData>())
        {
            Texture* shadowTexture =
                resources.get<Texture>(m_ShadowMap);

            if (shadowTexture &&
                !scene.shadowData.empty())
            {
                shadowTexture->Bind(20);
                m_Shader->setInt(
                    "shadowMap",
                    20
                );

                m_Shader->setInt(
                    "shadowCasterCount",
                    static_cast<int>(
                        scene.shadowData.size()
                    )
                );

                shadowsEnabled = true;
            }
        }

        m_Shader->setInt(
            "shadowsEnabled",
            shadowsEnabled ? 1 : 0
        );

        m_QuadVAO.Bind();

        glDrawArrays(
            GL_TRIANGLE_STRIP,
            0,
            4
        );

        m_QuadVAO.Unbind();

        output->unbind();
    }

    ResourceId framebuffer(){
        return m_FrameBuffer;
    }
private:
    Shader* m_Shader = nullptr;

    ResourceId m_FrameBuffer =
        INVALID_RESOURCE_ID;

    ResourceId m_GAlbedo =
        INVALID_RESOURCE_ID;
    ResourceId m_GLinDepth =
        INVALID_RESOURCE_ID;
    ResourceId m_GNormal =
        INVALID_RESOURCE_ID;
    ResourceId m_GARM =
        INVALID_RESOURCE_ID;
    ResourceId m_GEmissive =
        INVALID_RESOURCE_ID;    
    ResourceId m_ShadowMap =
        INVALID_RESOURCE_ID;

    ResourceId m_Output =
        INVALID_RESOURCE_ID;

    

    VAO m_QuadVAO;
    VBO m_QuadVBO;

    UniformBuffer* m_LightsUBO =
        new UniformBuffer(
            sizeof(GPULight) * 32 +
            sizeof(int) +
            12,
            1
        );

    UniformBuffer* m_LightMatricesUBO =
        new UniformBuffer(
            sizeof(Mat4) * 162,
            2
        );

    void uploadLights(
        const SceneRenderData& scene)
    {
        m_LightsUBO->bind();

        if (!scene.lights.empty())
        {
            m_LightsUBO->update(
                scene.lights.data(),
                scene.lights.size() * sizeof(GPULight),
                0
            );
        }

        const int lightCount =
            static_cast<int>(scene.lights.size());

        m_LightsUBO->update(
            &lightCount,
            sizeof(int),
            sizeof(GPULight) * 32
        );
    }

    void uploadLightMatrices(
        const SceneRenderData& scene)
    {
        m_LightMatricesUBO->bind();

        if (!scene.shadowData.empty())
        {
            m_LightMatricesUBO->update(
                scene.shadowData.data(),
                scene.shadowData.size() * sizeof(ShadowData),
                0
            );
        }
    }

    static constexpr float quadVertices[16] =
    {
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f
    };
};