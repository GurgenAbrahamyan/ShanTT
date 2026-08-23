#pragma once

#include <glad/glad.h>
#include <cstddef>
#include <span>

class StorageBuffer
{
public:

    StorageBuffer()
    {
        glGenBuffers(1, &m_handle);
    }

    ~StorageBuffer()
    {
        destroy();
    }

    StorageBuffer(const StorageBuffer&) = delete;
    StorageBuffer& operator=(const StorageBuffer&) = delete;

    StorageBuffer(StorageBuffer&& other) noexcept
        : m_handle(other.m_handle)
        , m_capacity(other.m_capacity)
    {
        other.m_handle = 0;
        other.m_capacity = 0;
    }

    StorageBuffer& operator=(StorageBuffer&& other) noexcept
    {
        if (this == &other)
            return *this;

        destroy();

        m_handle = other.m_handle;
        m_capacity = other.m_capacity;

        other.m_handle = 0;
        other.m_capacity = 0;

        return *this;
    }


    void upload(
        const void* data,
        std::size_t size,
        GLenum usage = GL_DYNAMIC_DRAW)
    {
        bind();

        if (size > m_capacity)
        {
            glBufferData(
                GL_SHADER_STORAGE_BUFFER,
                static_cast<GLsizeiptr>(size),
                data,
                usage
            );

            m_capacity = size;
        }
        else if (size > 0)
        {
            glBufferSubData(
                GL_SHADER_STORAGE_BUFFER,
                0,
                static_cast<GLsizeiptr>(size),
                data
            );
        }

        unbind();
    }
    


    void bind() const
    {
        glBindBuffer(
            GL_SHADER_STORAGE_BUFFER,
            m_handle
        );
    }


    static void unbind()
    {
        glBindBuffer(
            GL_SHADER_STORAGE_BUFFER,
            0
        );
    }


    void bindBase(
        GLuint binding
    ) const
    {
        glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER,
            binding,
            m_handle
        );
    }


    void bindRange(
        GLuint binding,
        std::size_t offset,
        std::size_t size
    ) const
    {
        glBindBufferRange(
            GL_SHADER_STORAGE_BUFFER,
            binding,
            m_handle,
            static_cast<GLintptr>(offset),
            static_cast<GLsizeiptr>(size)
        );
    }


    GLuint handle() const
    {
        return m_handle;
    }


    std::size_t capacity() const
    {
        return m_capacity;
    }


    void destroy()
    {
        if (m_handle == 0)
            return;

        glDeleteBuffers(1, &m_handle);

        m_handle = 0;
        m_capacity = 0;
    }


private:

    GLuint m_handle = 0;
    std::size_t m_capacity = 0;
};