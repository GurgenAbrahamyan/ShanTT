#include "RenderBuffer.h"
#include <iostream>

RenderBuffer::RenderBuffer(uint32_t width, uint32_t height, RenderBufferFormat format, uint32_t samples)
    : m_ID(0), m_Width(width), m_Height(height), m_Format(format), m_Samples(samples)
{
    glGenRenderbuffers(1, &m_ID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_ID);

    if (samples > 1)
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples,
            (format == RenderBufferFormat::Depth24) ? GL_DEPTH_COMPONENT24 :
            (format == RenderBufferFormat::Depth32F) ? GL_DEPTH_COMPONENT32F :
            (format == RenderBufferFormat::Depth24Stencil8) ? GL_DEPTH24_STENCIL8 :
            (format == RenderBufferFormat::Stencil8) ? GL_STENCIL_INDEX8 :
            GL_DEPTH_COMPONENT24, width, height);
    else
        glRenderbufferStorage(GL_RENDERBUFFER,
            (format == RenderBufferFormat::Depth24) ? GL_DEPTH_COMPONENT24 :
            (format == RenderBufferFormat::Depth32F) ? GL_DEPTH_COMPONENT32F :
            (format == RenderBufferFormat::Depth24Stencil8) ? GL_DEPTH24_STENCIL8 :
            (format == RenderBufferFormat::Stencil8) ? GL_STENCIL_INDEX8 :
            GL_DEPTH_COMPONENT24, width, height);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderBuffer::resize(uint32_t width, uint32_t height)
{
    m_Width = width;
    m_Height = height;

    glBindRenderbuffer(GL_RENDERBUFFER, m_ID);
    if (m_Samples > 1)
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Samples,
            (m_Format == RenderBufferFormat::Depth24) ? GL_DEPTH_COMPONENT24 :
            (m_Format == RenderBufferFormat::Depth32F) ? GL_DEPTH_COMPONENT32F :
            (m_Format == RenderBufferFormat::Depth24Stencil8) ? GL_DEPTH24_STENCIL8 :
            (m_Format == RenderBufferFormat::Stencil8) ? GL_STENCIL_INDEX8 :
            GL_DEPTH_COMPONENT24, m_Width, m_Height);
    else
        glRenderbufferStorage(GL_RENDERBUFFER,
            (m_Format == RenderBufferFormat::Depth24) ? GL_DEPTH_COMPONENT24 :
            (m_Format == RenderBufferFormat::Depth32F) ? GL_DEPTH_COMPONENT32F :
            (m_Format == RenderBufferFormat::Depth24Stencil8) ? GL_DEPTH24_STENCIL8 :
            (m_Format == RenderBufferFormat::Stencil8) ? GL_STENCIL_INDEX8 :
            GL_DEPTH_COMPONENT24, m_Width, m_Height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

RenderBuffer::RenderBuffer(RenderBuffer&& other) noexcept
    : m_ID(other.m_ID), m_Width(other.m_Width), m_Height(other.m_Height),
    m_Format(other.m_Format), m_Samples(other.m_Samples)
{
    other.m_ID = 0;
}

RenderBuffer& RenderBuffer::operator=(RenderBuffer&& other) noexcept
{
    if (this != &other)
    {
        glDeleteRenderbuffers(1, &m_ID);

        m_ID = other.m_ID;
        m_Width = other.m_Width;
        m_Height = other.m_Height;
        m_Format = other.m_Format;
        m_Samples = other.m_Samples;

        other.m_ID = 0;
    }
    return *this;
}

uint32_t RenderBuffer::getID() const { return m_ID; }
uint32_t RenderBuffer::getWidth() const { return m_Width; }
uint32_t RenderBuffer::getHeight() const { return m_Height; }
RenderBufferFormat RenderBuffer::getFormat() const { return m_Format; }
uint32_t RenderBuffer::getSamples() const { return m_Samples; }