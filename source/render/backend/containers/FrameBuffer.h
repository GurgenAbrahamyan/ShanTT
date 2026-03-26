#pragma once
#include <cstdint>
#include <glad/glad.h>
#include "RenderBuffer.h"


class FrameBuffer
{
private:
    GLuint m_ID = 0;
    std::vector<GLuint> m_ColorAttachments;
    GLuint m_DepthAttachment = 0;

    RenderBuffer m_DepthStencilBuffer;

    uint32_t m_Width = 0;
    uint32_t m_Height = 0;

public:
    FrameBuffer(uint32_t width, uint32_t height)
        : m_Width(width),
        m_Height(height)
    {
        glGenFramebuffers(1, &m_ID);
        bind();
        unbind();
    }

    ~FrameBuffer()
    {
        for (auto tex : m_ColorAttachments)
            glDeleteTextures(1, &tex);
        if (m_DepthAttachment)
            glDeleteTextures(1, &m_DepthAttachment);
        if (m_ID)
            glDeleteFramebuffers(1, &m_ID);
    }

    void bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
    }

    void unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;

        m_Width = width;
        m_Height = height;

        for (auto tex : m_ColorAttachments) {
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, nullptr);
        }

        if (m_DepthAttachment) {
            glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
                m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }
    }

    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    GLuint getID() const { return m_ID; }
    GLuint getColorAttachment(size_t index) const {
        if (index < m_ColorAttachments.size()) return m_ColorAttachments[index];
        else {
            return getDepthAttachment();
        }
        
    }

	int getColorAttachmentCount() const { return (int)m_ColorAttachments.size(); }
    GLuint getDepthStencilBuffer() const { return m_DepthStencilBuffer.getID(); }
    GLuint getDepthAttachment() const { return m_DepthAttachment; }

    void addColorBuffer(GLenum internalFormat = GL_RGB16F, GLenum format = GL_RGBA, GLenum type = GL_FLOAT)
    {
        bind();

        GLuint colorTex;
        glGenTextures(1, &colorTex);
        glBindTexture(GL_TEXTURE_2D, colorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, format, type, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

       
        GLenum attachment = GL_COLOR_ATTACHMENT0 + m_ColorAttachments.size();
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, colorTex, 0);

        m_ColorAttachments.push_back(colorTex);

        
        std::vector<GLenum> drawBuffers;
        for (size_t i = 0; i < m_ColorAttachments.size(); ++i)
            drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
        glDrawBuffers(drawBuffers.size(), drawBuffers.data());

        unbind();
    }
    void addDepthBuffer() {
        bind();
        glGenTextures(1, &m_DepthAttachment);
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_Width, m_Height, 0,
            GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D, m_DepthAttachment, 0);
        unbind();
    }

    void disableColor() {
      
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        
    }

    GLuint getWidth() const { return m_Width; }
    GLuint getHeight() const { return m_Height; }
};