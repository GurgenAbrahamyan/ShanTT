#pragma once

#include <cassert>
#include <cstdint>
#include <vector>

#include <glad/glad.h>



class FrameBuffer
{
private:
    GLuint m_ID = 0;

    std::vector<GLuint> m_ColorAttachments;
    GLuint m_DepthAttachment = 0;

    uint32_t m_Width = 0;
    uint32_t m_Height = 0;

public:
    FrameBuffer(uint32_t width, uint32_t height)
        : m_Width(width),
          m_Height(height)
    {
        glGenFramebuffers(1, &m_ID);
    }

    ~FrameBuffer()
    {
        if (m_ID)
            glDeleteFramebuffers(1, &m_ID);
    }

    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    void bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
    }

    void unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void attachColor(GLuint textureID, size_t index)
    {
        bind();

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(index),
            GL_TEXTURE_2D,
            textureID,
            0
        );

        if (index >= m_ColorAttachments.size())
            m_ColorAttachments.resize(index + 1, 0);

        m_ColorAttachments[index] = textureID;

        refreshDrawBuffers();

        unbind();
    }

    void attachDepth(GLuint textureID)
    {
        bind();

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D,
            textureID,
            0
        );

        m_DepthAttachment = textureID;

        unbind();
    }

    void detachColor(size_t index)
    {
        if (index >= m_ColorAttachments.size())
            return;

        bind();

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(index),
            GL_TEXTURE_2D,
            0,
            0
        );

        m_ColorAttachments[index] = 0;

        refreshDrawBuffers();

        unbind();
    }

    void detachDepth()
    {
        bind();

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D,
            0,
            0
        );

        m_DepthAttachment = 0;

        unbind();
    }

    void setExtent(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
    }

    bool isComplete() const
    {
        bind();

        const GLenum status =
            glCheckFramebufferStatus(GL_FRAMEBUFFER);

        unbind();

        return status == GL_FRAMEBUFFER_COMPLETE;
    }

    GLuint getID() const
    {
        return m_ID;
    }

    GLuint getColorAttachment(size_t index) const
    {
        assert(
            index < m_ColorAttachments.size() &&
            "FrameBuffer: color attachment index out of range"
        );

        return m_ColorAttachments[index];
    }

    size_t getColorAttachmentCount() const
    {
        return m_ColorAttachments.size();
    }

    GLuint getDepthAttachment() const
    {
        return m_DepthAttachment;
    }

    uint32_t getWidth() const
    {
        return m_Width;
    }

    uint32_t getHeight() const
    {
        return m_Height;
    }

private:
    void refreshDrawBuffers()
{
    std::vector<GLenum> drawBuffers;

    for (size_t i = 0; i < m_ColorAttachments.size(); ++i)
    {
        if (m_ColorAttachments[i] != 0)
        {
            drawBuffers.push_back(
                GL_COLOR_ATTACHMENT0 +
                static_cast<GLenum>(i)
            );
        }
    }

    if (drawBuffers.empty())
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    else
    {
        glDrawBuffers(
            static_cast<GLsizei>(drawBuffers.size()),
            drawBuffers.data()
        );
    }
}
};