#pragma once
#include "RenderPass.h"
#include "../ecs_systems/ShadowAtlas.h"

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