#pragma once
#include "../data/RenderContext.h"
#include "../data/BatchMap.h"
#include "../backend/containers/GlobalUniformBuffer.h"
#include "../data/GPULight.h"
#include "../../math_custom/Mat4.h"
#include "../../ecs/components/core/TransformComponent.h"
#include "../../ecs/components/core/ParentComponent.h"
#include "../../ecs/components/graphics/MeshComponent.h"
#include "../../ecs/components/graphics/MaterialComponent.h"
#include "../../ecs/components/graphics/LightComponent.h"
#include "../../ecs/components/graphics/CubeMapComponent.h"
#include "../../ecs/components/graphics/CameraComponent.h"
#include "../../ecs/components/graphics/ActiveCameraTag.h"
#include "../../ecs/components/core/WorldMatrixComponent.h"
#include "../../ecs/components/graphics/ModelComponent.h"
#include "../backend/Shader.h"

#include "../ecs_systems/ShadowAtlas.h"
#include "../data/RenderResource.h"
#include <glad/glad.h>


class RenderPass
{
protected:
    Shader* shader;

public:
    std::vector<RenderResource*> inputs;
    std::vector<RenderResource*> outputs;

    RenderPass(Shader* s) : shader(s) {}

    virtual void execute(RenderContext& ctx) = 0;
};




class ShadowPass : public RenderPass
{
public:
    ShadowPass(Shader* s)
        : RenderPass(s)
    {

    }

    void execute(RenderContext& ctx) override {
        if (outputs.empty() || ctx.shadowCasters.empty()) return;

        int numCasters = (int)ctx.shadowData.size();


        FrameBuffer* fb = outputs[0]->framebuffer;
        if (fb->getWidth() != ShadowAtlas::ATLAS_SIZE || fb->getHeight() != ShadowAtlas::ATLAS_SIZE) {
            fb->resize(ShadowAtlas::ATLAS_SIZE, ShadowAtlas::ATLAS_SIZE);

        }

        shader->Activate();
        fb->bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);


        for (ShadowData& data : ctx.shadowData) {

            int x = int(data.uvMin.x * ShadowAtlas::ATLAS_SIZE);
            int y = int(data.uvMin.y * ShadowAtlas::ATLAS_SIZE);
            int w = int((data.uvMax.x - data.uvMin.x) * ShadowAtlas::ATLAS_SIZE);
            int h = int((data.uvMax.y - data.uvMin.y) * ShadowAtlas::ATLAS_SIZE);

            glViewport(x, y, w, h);
            shader->setMat4("lightSpaceMatrix", data.lightMatrix);


            for (auto& [mat, meshMap] : ctx.batches) {
                for (auto& [mesh, batch] : meshMap) {
                    if (batch.instances.empty()) continue;
                    mesh->bind();
                    mesh->setupInstanceVBO(batch.instances.size());
                    glBindBuffer(GL_ARRAY_BUFFER, mesh->getInstanceVBO());
                    glBufferSubData(GL_ARRAY_BUFFER, 0,
                        batch.instances.size() * sizeof(Mat4),
                        batch.instances.data());
                    glDrawElementsInstanced(GL_TRIANGLES,
                        mesh->indexCount(), GL_UNSIGNED_INT, 0,
                        batch.instances.size());
                }
            }

        }

        fb->unbind();
        glViewport(0, 0, ctx.windowWidth, ctx.windowHeight);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }






};
class GeometryPass : public RenderPass {
public:
    GeometryPass(Shader* s) : RenderPass(s) {}

    void execute(RenderContext& ctx) override
    {
        if (outputs.empty()) return;
        if (!ctx.camera)
            return;


        glDisable(GL_CULL_FACE);






        FrameBuffer* fb = outputs[0]->framebuffer;
        fb->bind();
        glViewport(0, 0, fb->getWidth(), fb->getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_CULL_FACE);
        shader->Activate();

        shader->setMat4("view", ctx.camera->viewMatrix);
        shader->setMat4("projection", ctx.camera->projectionMatrix);

        for (auto& [mat, meshMap] : ctx.batches) {
            mat->Bind(shader);
            for (auto& [mesh, batch] : meshMap) {
                if (batch.instances.empty()) continue;
                mesh->bind();
                mesh->setupInstanceVBO(batch.instances.size());
                glBindBuffer(GL_ARRAY_BUFFER, mesh->getInstanceVBO());
                glBufferSubData(GL_ARRAY_BUFFER, 0, batch.instances.size() * sizeof(Mat4), batch.instances.data());
                glDrawElementsInstanced(GL_TRIANGLES, mesh->indexCount(), GL_UNSIGNED_INT, 0, batch.instances.size());
            }
        }

        fb->unbind();
    }


private:
    UniformBuffer* lightsUBO = new UniformBuffer(
        sizeof(GPULight) * 32 + sizeof(int) + 12, 1
    );
    UniformBuffer* lightMatricesUBO = new UniformBuffer(
        sizeof(Mat4) * 162, 2);

    void uploadLights(RenderContext* ctx)
    {
        auto& gpuLights = ctx->lights;

        lightsUBO->bind();

        if (!gpuLights.empty())
        {
            lightsUBO->update(
                gpuLights.data(),
                gpuLights.size() * sizeof(GPULight),
                0
            );

        }

        int lightCount = (int)gpuLights.size();
        lightsUBO->update(&lightCount, sizeof(int), sizeof(GPULight) * 32);
    }

    void uploadLightMatrices(RenderContext* ctx)
    {
        auto& gpuLights = ctx->shadowData;



        lightMatricesUBO->bind();

        if (!gpuLights.empty())
        {
            lightMatricesUBO->update(
                gpuLights.data(),
                gpuLights.size() * sizeof(ShadowData),
                0
            );

        }


    }
};

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

    void execute(RenderContext& ctx) override {
        if (outputs.empty() || inputs.empty()) return;
        if (!ctx.camera)
            return;
        FrameBuffer* fb = outputs[0]->framebuffer;
        fb->bind();
        glViewport(0, 0, fb->getWidth(), fb->getHeight());

        shader->Activate();


        shader->setVec3("cameraPos", ctx.cameraTransform->position);


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

        if (ctx.cubeMapComp) {
            glActiveTexture(GL_TEXTURE10);
            ctx.cubeMapComp->cubeMap->bindIrrTexture(10);
            glActiveTexture(GL_TEXTURE11);
            ctx.cubeMapComp->cubeMap->bindPreFilterTexture(11);
            glActiveTexture(GL_TEXTURE12);
            ctx.brdfTexture->Bind(12);

            shader->setFloat("envIntensity", ctx.cubeMapComp->intensity);

        }
        shader->setInt("gAlbedo", 0);
        shader->setInt("gPosition", 1);
        shader->setInt("gNormal", 2);
        shader->setInt("gARM", 3);
        shader->setInt("gEmissive", 4);
        shader->setInt("irradianceMap", 10);
        shader->setInt("prefilterMap", 11);
        shader->setInt("brdfLUT", 12);


        uploadLights(&ctx);
        uploadLightMatrices(&ctx);


        if (inputs.size() > 1) {
            glActiveTexture(GL_TEXTURE20);
            glBindTexture(GL_TEXTURE_2D, inputs[1]->framebuffer->getDepthAttachment());
            shader->setInt("shadowsEnabled", 1);
            shader->setInt("shadowMap", 20);
            shader->setInt("shadowCasterCount", (int)ctx.shadowData.size());
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

    void uploadLights(RenderContext* ctx) {
        lightsUBO->bind();
        if (!ctx->lights.empty()) {
            lightsUBO->update(ctx->lights.data(), ctx->lights.size() * sizeof(GPULight), 0);
        }
        int lightCount = (int)ctx->lights.size();
        lightsUBO->update(&lightCount, sizeof(int), sizeof(GPULight) * 32);
    }

    void uploadLightMatrices(RenderContext* ctx) {
        lightMatricesUBO->bind();
        if (!ctx->shadowData.empty()) {
            lightMatricesUBO->update(ctx->shadowData.data(), ctx->shadowData.size() * sizeof(ShadowData), 0);
        }
    }

    static constexpr float quadVertices[16] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
    };
};


class CubeMapPass : public RenderPass {
public:
    CubeMapPass(Shader* s) : RenderPass(s) {

    }

    void execute(RenderContext& context) override {

        if (outputs.empty() || inputs.empty()) return;
        FrameBuffer* fb = outputs[0]->framebuffer;
        fb->bind();
        glViewport(0, 0, fb->getWidth(), fb->getHeight());

        //   glDisable(GL_CULL_FACE);


        glBindFramebuffer(GL_READ_FRAMEBUFFER, inputs[0]->framebuffer->getID());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->getID());
        glBlitFramebuffer(
            0, 0, inputs[0]->framebuffer->getWidth(), inputs[0]->framebuffer->getHeight(),
            0, 0, fb->getWidth(), fb->getHeight(),
            GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );

        auto* cam = context.camera;
        auto* camTrans = context.cameraTransform;
        if (!cam || !camTrans) return;


        if (!context.cubeMapComp) return;
        CubeMapComponent& cmc = *context.cubeMapComp;
        if (!cmc.cubeMap) return;

        shader->Activate();

        glDepthFunc(GL_LEQUAL);


        Mat4 view_mat = cam->viewMatrix;


        shader->setInt("skybox", 0);
        shader->setMat4("projection", cam->projectionMatrix);
        shader->setFloat("envIntensity", cmc.intensity);
        shader->setMat4("view", view_mat);

        cmc.cubeMap->bind();
        cmc.cubeMap->bindEnvTexture(0);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthFunc(GL_LESS);
        // glEnable(GL_CULL_FACE);
        fb->unbind();
    }
};

class BlurPass : public RenderPass {
public:
    BlurPass(Shader* s)
        : RenderPass(s),
        quadVBO(quadVertices, sizeof(quadVertices), false)
    {

        quadVAO.Bind();
        quadVBO.Bind();

        quadVAO.LinkAttrib(quadVBO, 0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
        quadVAO.LinkAttrib(quadVBO, 1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        quadVAO.Unbind();
        quadVBO.Unbind();
    }

    bool alreadySwapped = false;

    void execute(RenderContext& context) override {


        auto* cam = context.camera;
        if (!cam || inputs.empty() || outputs.empty()) return;

        if (!blurFB) blurFB = outputs[0]->framebuffer;

        if (!cam->applyBlur) {
            outputs[0]->framebuffer = inputs[0]->framebuffer;
            return;
        }

        outputs[0]->framebuffer = blurFB;



        FrameBuffer* inputFB = inputs[0]->framebuffer;
        FrameBuffer* outputFB = outputs[0]->framebuffer;

        outputFB->bind();

        shader->Activate();

        shader->setInt("uSceneTexture", 0);
        shader->setInt("uDepthTexture", 1);

        shader->setFloat("uFocusDistance", cam->focusDistance);
        shader->setFloat("uAperture", cam->aperture);
        shader->setFloat("uFocalLength", cam->focalLength);
        shader->setFloat("uBlurScale", cam->blurScale);
        shader->setFloat("uNearPlane", cam->nearPlane);
        shader->setFloat("uFarPlane", cam->farPlane);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputFB->getColorAttachment(0));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, inputFB->getDepthAttachment());

        quadVAO.Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        quadVAO.Unbind();

        outputFB->unbind();
    }
private:
    FrameBuffer* blurFB = nullptr;
    VAO quadVAO;
    VBO quadVBO;

    static constexpr float quadVertices[16] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    };
};

class BloomPass : public RenderPass {
public:
    BloomPass(Shader* extractShader, Shader* blurShader, int width, int height)
        : RenderPass(blurShader), extractShader(extractShader),

        quadVBO(quadVertices, sizeof(quadVertices), false)
    {
        pingpongFBO[0] = new FrameBuffer(width, height);
        pingpongFBO[0]->addColorBuffer();
        pingpongFBO[1] = new FrameBuffer(width, height);
        pingpongFBO[1]->addColorBuffer();

        quadVAO.Bind();
        quadVBO.Bind();
        quadVAO.LinkAttrib(quadVBO, 0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
        quadVAO.LinkAttrib(quadVBO, 1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        quadVAO.Unbind();
        quadVBO.Unbind();
    }

    void execute(RenderContext& context) override
    {
        if (inputs.empty() || outputs.empty()) return;

        FrameBuffer* inputFB = inputs[0]->framebuffer;
        glViewport(0, 0, inputFB->getWidth(), inputFB->getHeight());


        for (int i = 0; i < 2; i++) {
            pingpongFBO[i]->bind();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            pingpongFBO[i]->unbind();
        }



        extractShader->Activate();
        pingpongFBO[0]->bind();

        glClear(GL_COLOR_BUFFER_BIT);

        extractShader->setInt("sceneTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputFB->getColorAttachment(0));

        quadVAO.Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        quadVAO.Unbind();

        pingpongFBO[0]->unbind();

        bool horizontal = true, first_iteration = true;
        int amount = 10;
        shader->Activate();
        for (unsigned int i = 0; i < amount; i++)
        {
            pingpongFBO[horizontal]->bind();
            glViewport(0, 0,
                pingpongFBO[horizontal]->getWidth(),
                pingpongFBO[horizontal]->getHeight());
            shader->setInt("horizontal", horizontal);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(
                GL_TEXTURE_2D, first_iteration ? pingpongFBO[0]->getColorAttachment(0)
                : pingpongFBO[!horizontal]->getColorAttachment(0)
            );
            shader->setInt("sceneTexture", 0);
            quadVAO.Bind();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            quadVAO.Unbind();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        pingpongFBO[!horizontal]->unbind();



        outputs[0]->framebuffer = pingpongFBO[!horizontal];
    }

private:
    VAO quadVAO;
    VBO quadVBO;
    FrameBuffer* pingpongFBO[2];
    Shader* extractShader;

    static constexpr float quadVertices[16] = {
  -1.0f,  1.0f,  0.0f, 1.0f,
  -1.0f, -1.0f,  0.0f, 0.0f,
   1.0f,  1.0f,  1.0f, 1.0f,
   1.0f, -1.0f,  1.0f, 0.0f,
    };
};

class FinalBlitPass : public RenderPass {

public:
    FinalBlitPass(Shader* shader)
        : RenderPass(shader),
        quadVBO(quadVertices, sizeof(quadVertices), false)
    {

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

    void execute(RenderContext& context) override
    {
        if (inputs.size() == 0) return;

        FrameBuffer* sceneFB = inputs[0]->framebuffer;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);

        shader->Activate();
        shader->setInt("screenTexture", 0);

        shader->setFloat("exposure", 2);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneFB->getColorAttachment(0));
        // glBindTexture(GL_TEXTURE_2D, context.brdfTexture->getID());


        // context.brdfTexture->Bind(0);
        if (inputs.size() > 1) {
            FrameBuffer* bloomFB = inputs[1]->framebuffer;
            shader->setInt("bloomTexture", 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, bloomFB->getColorAttachment(0));
        }
        else {
            shader->setInt("isBloom", false);
        }
        quadVAO.Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        quadVAO.Unbind();

        glEnable(GL_DEPTH_TEST);
    }

private:
    VAO quadVAO;
    VBO quadVBO;

    static constexpr float quadVertices[16] = {
     -1.0f,  1.0f,  0.0f, 1.0f,
     -1.0f, -1.0f,  0.0f, 0.0f,
      1.0f,  1.0f,  1.0f, 1.0f,
      1.0f, -1.0f,  1.0f, 0.0f,
    };
};