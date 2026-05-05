#pragma once
#include "RenderPass.h"


class FinalBlitPass : public RenderPass {

    static struct Settings {

        float exposure = 0.8; //0.8 - 1.3
        bool isBloom = true;
		float bloomintensity = 0.06f;

    };

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

        if (outputs.empty()){
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
        else {
            outputs[0]->framebuffer->bind();
        }

        glDisable(GL_DEPTH_TEST);

        shader->Activate();
        shader->setInt("screenTexture", 0);

        shader->setFloat("exposure", settings.exposure);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneFB->getColorAttachment(0));
        //glBindTexture(GL_TEXTURE_2D, context.brdfTexture->getID());


        // context.brdfTexture->Bind(0);
        if (inputs.size() > 1) {
            FrameBuffer* bloomFB = inputs[1]->framebuffer;
            shader->setInt("bloomTexture", 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, bloomFB->getColorAttachment(0));
			shader->setInt("isBloom", settings.isBloom);
			shader->setFloat("bloomStrength", settings.bloomintensity);
        }
        
        quadVAO.Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        quadVAO.Unbind();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      
        glEnable(GL_DEPTH_TEST);

    }

    const char* passName() const override { return "Final Blit"; }

    Settings settings;

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