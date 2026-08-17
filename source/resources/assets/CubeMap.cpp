#include "CubeMap.h"

CubeMap::CubeMap(int size, GLenum internalFormat, int mipLevels)
    : size(size),  mipLevels(mipLevels), internalFormat(internalFormat) {
        
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    for (int face = 0; face < 6; ++face) {
        for (int mip = 0; mip < mipLevels; ++mip) {
            int mipSize = size >> mip;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip,
                         internalFormat, mipSize, mipSize, 0,
                         GL_RGB, GL_FLOAT, nullptr);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                     mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMap::CubeMap(CubeMap&& other) noexcept
    : ID(other.ID), size(other.size), mipLevels(other.mipLevels), internalFormat(other.internalFormat) {
    other.ID = 0;
}

CubeMap& CubeMap::operator=(CubeMap&& other) noexcept {
    if (this != &other) {
        if (ID) glDeleteTextures(1, &ID);
        ID = other.ID;
        size = other.size;
        mipLevels = other.mipLevels;
        internalFormat = other.internalFormat;
        other.ID = 0;
    }
    return *this;
}

CubeMap::~CubeMap() {
    if (ID) glDeleteTextures(1, &ID);
}

void CubeMap::bind(int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void CubeMap::unbind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMap::uploadFace(int faceIndex, int mip, GLenum format, GLenum type, const void* data) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    int mipSize = size >> mip;
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, mip,
                 internalFormat, mipSize, mipSize, 0, format, type, data);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}