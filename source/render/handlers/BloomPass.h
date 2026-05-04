#pragma once
#include "RenderPass.h"
#include <glfw/glfw3.h>
#include <glad/glad.h>
class BloomPass : public RenderPass {

    static struct Settings {

        int mipMapLength = 5;
        float filterRadius = 0.005;

    };
public:
    BloomPass(Shader* downScaleShader, Shader* upScaleShader, int width, int height)
        : RenderPass(downScaleShader), upsampleShader(upScaleShader),
        windowWidth(width), windowHeight(height),
        quadVBO(quadVertices, sizeof(quadVertices), false)
    {
        init(width, height, settings.mipMapLength);


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

        bloomFB->bind();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        bloomFB->unbind();

        downscalePass(inputs[0]->framebuffer);
        upscalePass(outputs[0], settings.filterRadius);

    }

    Settings settings;

private:
    
    float windowWidth, windowHeight;
    struct mipLevel {
        Texture* texture;
        Vector2 size;
    };

    VAO quadVAO;
    VBO quadVBO;

    Shader* upsampleShader;
    FrameBuffer* bloomFB;
    std::vector<mipLevel> mipLevels;

    static constexpr float quadVertices[16] = {
  -1.0f,  1.0f,  0.0f, 1.0f,
  -1.0f, -1.0f,  0.0f, 0.0f,
   1.0f,  1.0f,  1.0f, 1.0f,
   1.0f, -1.0f,  1.0f, 0.0f,
    };



    void init(int windowWidth, int windowHeight, int mipMapLength) {

        bloomFB = new FrameBuffer(windowWidth, windowHeight);
        bloomFB->bind();

        Vector2 initialSizeXY = Vector2(windowWidth, windowHeight);

        for (int i = 0; i < mipMapLength; i++) {
            mipLevel level;
            level.size = initialSizeXY / pow(2, i);
            TextureDesc desc;
            desc.internalFormat = GL_R11F_G11F_B10F;
            desc.format = GL_RGB;
            desc.type = GL_FLOAT;

            desc.generateMipmaps = false;

            desc.minFilter = GL_LINEAR;
            desc.magFilter = GL_LINEAR;

            desc.wrapS = GL_CLAMP_TO_EDGE;
            desc.wrapT = GL_CLAMP_TO_EDGE;

            level.texture = new Texture(
                (int)level.size.x,
                (int)level.size.y,
                nullptr,
                desc);

            mipLevels.push_back(level);
        }


        bloomFB->addColorAttachmentFromTexture(mipLevels[0].texture->getID());

        bloomFB->unbind();




    }
    void downscalePass(FrameBuffer* inputFB) {


        bloomFB->bind();
        Shader* downsampleShader = shader;


        downsampleShader->Activate();
        downsampleShader->setVec2("srcResolution", { windowWidth, windowHeight });


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputFB->getColorAttachment(0));


        for (int i = 0; i < mipLevels.size(); i++)
        {
            const mipLevel& mip = mipLevels[i];
            glViewport(0, 0, mip.size.x, mip.size.y);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D, mip.texture->getID(), 0);

            quadVAO.Bind();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);

            downsampleShader->setVec2("srcResolution", mip.size);

            mip.texture->Bind(0);
        }

        bloomFB->unbind();
    }

    void upscalePass(RenderResource* outputFB, float filterRadius) {

        bloomFB->bind();

        upsampleShader->Activate();
        upsampleShader->setFloat("filterRadius", filterRadius);


        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);

        for (int i = mipLevels.size() - 1; i > 0; i--)
        {
            const mipLevel& mip = mipLevels[i];
            const mipLevel& nextMip = mipLevels[i - 1];


            mip.texture->Bind(0);


            glViewport(0, 0, nextMip.size.x, nextMip.size.y);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D, nextMip.texture->getID(), 0);


            quadVAO.Bind();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            quadVAO.Unbind();
        }
        glDisable(GL_BLEND);

        bloomFB->unbind();
        outputFB->framebuffer = bloomFB;
        bloomFB->setColorAttachment(0, mipLevels[0].texture->getID());


    }

    const char* passName() const override { return "Bloom"; }
};
