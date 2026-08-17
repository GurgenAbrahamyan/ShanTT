#pragma once
#include <glad/glad.h>

class CubeMap {
public:
    CubeMap(int size, GLenum internalFormat, int mipLevels = 1);
    CubeMap(const CubeMap&) = delete;
    CubeMap& operator=(const CubeMap&) = delete;
    CubeMap(CubeMap&& other) noexcept;
    CubeMap& operator=(CubeMap&& other) noexcept;

    void bind(int slot = 0) const;
    void unbind() const;

    void uploadFace(int faceIndex, int mip, GLenum format, GLenum type, const void* data);

    int getSize() const { return size; }
    int getMipLevels() const { return mipLevels; }
    GLuint getID() const { return ID; }

    ~CubeMap();

private:
    GLuint ID = 0;
    int size = 0;
    int mipLevels = 1;
    GLenum internalFormat = GL_RGB16F;
};