#pragma once
#include <cstdint>
#include <glad/glad.h>
#include "RenderBuffer.h"

class FrameBuffer
{
private:
    GLuint m_ID = 0;
    GLuint m_ColorAttachment = 0;
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
        if (m_ColorAttachment)
            glDeleteTextures(1, &m_ColorAttachment);
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

        if (m_ColorAttachment) {
            glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
                m_Width, m_Height,
                0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
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
    GLuint getColorAttachment() const { return m_ColorAttachment ? m_ColorAttachment : m_DepthAttachment; }
    GLuint getDepthStencilBuffer() const { return m_DepthStencilBuffer.getID(); }
    GLuint getDepthAttachment() const { return m_DepthAttachment; }

    void addColorBuffer() {
        bind();
        glGenTextures(1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
            m_Width, m_Height,
            0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            m_ColorAttachment,
            0);
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