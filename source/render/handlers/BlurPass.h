#pragma once
#include "RenderPass.h"
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