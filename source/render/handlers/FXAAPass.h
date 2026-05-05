#pragma once

#include "RenderPass.h"

class FXAAPass : public RenderPass {

    struct Settings {
        float edgeThreshold = 0.1f;
        float blendStrength = 0.5f;
    };
    
public :
    FXAAPass(Shader* s)
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

   
    Settings settings;

    void execute(RenderContext& context) override {


        
        

        if (inputs.empty())
            return;
        
       



        FrameBuffer* inputFB = inputs[0]->framebuffer;

        glViewport(0, 0, inputFB->getWidth(), inputFB->getHeight());
       

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        shader->Activate();

        shader->setInt("screenTex", 0);
		shader->setFloat("threshold", settings.edgeThreshold);
		shader->setFloat("blendStrength", settings.blendStrength);
       

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputFB->getColorAttachment(0));
 

        quadVAO.Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        quadVAO.Unbind();

        
    }
	const char* passName() const override { return "FXAAPass"; }
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
