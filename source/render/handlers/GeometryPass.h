#pragma once
#include "RenderPass.h"
#include "../backend/containers/FrameBuffer.h"


class GeometryPass : public RenderPass {
public:
    GeometryPass(Shader* s) : RenderPass(s) {}

    void execute(const FrameRenderData& frameData, 
                const EngineResources&, 
                const DebugRenderData&) override
    {
        if (outputs.empty()) return;

        
        if (!frameData.camera)
            return;
        CameraComponent& camera {*frameData.camera};

        glDisable(GL_CULL_FACE);

        FrameBuffer* fb = outputs[0]->framebuffer;
        fb->bind();
        glViewport(0, 0, fb->getWidth(), fb->getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_CULL_FACE);
        shader->Activate();

        shader->setMat4("view", frameData.camera->viewMatrix);
        shader->setMat4("projection", camera.projectionMatrix);

        for (auto& [mat, meshMap] : frameData.batches) {
            mat->Bind(shader);
            for (auto& [mesh, batch] : meshMap) {
                if (batch.instances.empty()) continue;
                mesh->bind();
                mesh->setupInstanceVBO(batch.instances.size());
                glBindBuffer(GL_ARRAY_BUFFER, mesh->getInstanceVBO());
                glBufferSubData(GL_ARRAY_BUFFER, 0, batch.instances.size() * sizeof(Mat4), batch.instances.data());
                glDrawElementsInstanced(GL_TRIANGLES, mesh->indexCount(), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(batch.instances.size()));
            }
        }

        fb->unbind();
    }
};
