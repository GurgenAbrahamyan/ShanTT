#pragma once

#include<glad/glad.h>
#include <cstddef>
#include <iostream>

class UniformBuffer {
public:
    
    UniformBuffer(size_t size, GLuint binding)
        : binding(binding), bufferSize(size)
    {
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        std::cout << "UniformBuffer created (size: " << bufferSize << " bytes, binding: " << binding << ")\n";
    }

    
    ~UniformBuffer()
    {
        glDeleteBuffers(1, &ubo);
    }

    
    void update(const void* data, size_t size, size_t offset = 0)
    {
        if (offset + size > bufferSize) {
            std::cerr << "UniformBuffer update overflow!\n";
            return;
        }

        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

 
    void bind() const
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo);
    }

    void unbind() const
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, 0);
	}

    GLuint getID() const { return ubo; }

private:
    GLuint ubo = 0;
    GLuint binding = 0;
    size_t bufferSize = 0;
};
