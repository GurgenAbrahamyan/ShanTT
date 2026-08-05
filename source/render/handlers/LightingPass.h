#pragma once
#include "RenderPass.h"
#include "render/backend/containers/GlobalUniformBuffer.h"
class LightingPass : public RenderPass {
public:
    LightingPass(Shader* s) : RenderPass(s), quadVBO(quadVertices, sizeof(quadVertices), false) {

        quadVAO.Bind();
        quadVBO.Bind();

        quadVAO.LinkAttrib(quadVBO, 0, 2, GL_FLOAT,
            4 * sizeof(float), (void*)0);

        quadVAO.LinkAttrib(quadVBO, 1, 2, GL_FLOAT,
            4 * sizeof(float),
            (void*)(2 * sizeof(float)));

        quadVAO.Unbind();
        quadVBO.Unbind();
    }

    void execute(const FrameRenderData& frameData, 
                const EngineResources& resources, 
                const DebugRenderData&) override {
        if (outputs.empty() || inputs.empty()) return;
        if (!frameData.camera)
            return;
        const CameraComponent& camera {*frameData.camera};
        FrameBuffer* fb = outputs[0]->framebuffer;
        fb->bind();
        glViewport(0, 0, fb->getWidth(), fb->getHeight());

        shader->Activate();


        shader->setVec3("cameraPos", frameData.cameraTransform->position);
        shader->setMat4("invView", camera.viewMatrix.inverse());
        shader->setMat4("invProjection", camera.projectionMatrix.inverse());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputs[0]->framebuffer->getColorAttachment(0)); // gAlbedo
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, inputs[0]->framebuffer->getColorAttachment(1)); // gPos
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, inputs[0]->framebuffer->getColorAttachment(2)); // gNormal
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, inputs[0]->framebuffer->getColorAttachment(3)); // gARM
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, inputs[0]->framebuffer->getColorAttachment(4)); // gEmissive

        if (frameData.cubeMapComp) {
            const CubeMapComponent& cubeMap {*frameData.cubeMapComp};

            glActiveTexture(GL_TEXTURE10);
            cubeMap.cubeMap->bindIrrTexture(10);
            glActiveTexture(GL_TEXTURE11);
            cubeMap.cubeMap->bindPreFilterTexture(11);
            glActiveTexture(GL_TEXTURE12);
            resources.brdfTexture->Bind(12);

            shader->setFloat("envIntensity", frameData.cubeMapComp->intensity * cubeMap.dirLightInfluence);

        }
        shader->setInt("gAlbedo", 0);
        shader->setInt("gDepth", 1);
        shader->setInt("gNormal", 2);
        shader->setInt("gARM", 3);
        shader->setInt("gEmissive", 4);
        shader->setInt("irradianceMap", 10);
        shader->setInt("prefilterMap", 11);
        shader->setInt("brdfLUT", 12);


        uploadLights(frameData);
        uploadLightMatrices(frameData);


        if (inputs.size() > 1) {
            glActiveTexture(GL_TEXTURE20);
            glBindTexture(GL_TEXTURE_2D, inputs[1]->framebuffer->getDepthAttachment());
            shader->setInt("shadowsEnabled", 1);
            shader->setInt("shadowMap", 20);
            shader->setInt("shadowCasterCount", static_cast<int> (frameData.shadowData.size()));
        }
        else {
            shader->setInt("shadowsEnabled", 0);
        }


        quadVAO.Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        quadVAO.Unbind();

        fb->unbind();
    }

private:
    VAO quadVAO;
    VBO quadVBO;

    UniformBuffer* lightsUBO = new UniformBuffer(sizeof(GPULight) * 32 + sizeof(int) + 12, 1);
    UniformBuffer* lightMatricesUBO = new UniformBuffer(sizeof(Mat4) * 162, 2);

    void uploadLights(const FrameRenderData& frameData) {

        lightsUBO->bind();

        if (!frameData.lights.empty()) 
            lightsUBO->update(frameData.lights.data(), frameData.lights.size() * sizeof(GPULight), 0);
        

        auto lightCount = static_cast<size_t>(frameData.lights.size());
        lightsUBO->update(&lightCount, sizeof(int), sizeof(GPULight) * 32);
    }

    void uploadLightMatrices(const FrameRenderData& frameData) {

        lightMatricesUBO->bind();

        if (frameData.shadowData.empty()) 
            lightMatricesUBO->update(frameData.shadowData.data(), frameData.shadowData.size() * sizeof(ShadowData), 0);
        
    }

    static constexpr float quadVertices[16] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
    };
};
