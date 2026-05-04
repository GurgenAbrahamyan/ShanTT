#pragma once
#include "RenderPass.h"
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