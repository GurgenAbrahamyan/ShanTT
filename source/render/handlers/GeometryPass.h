#pragma once
#include "RenderPass.h"
#include "../backend/containers/GlobalUniformBuffer.h"
#include "../backend/containers/FrameBuffer.h"
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
