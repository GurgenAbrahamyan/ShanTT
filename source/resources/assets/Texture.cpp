#pragma once
#include "Texture.h"
#include <iostream>


Texture::Texture(int w, int h, const void* data, const TextureDesc& desc)
    : width(w), height(h), target(desc.target)
{
    glGenTextures(1, &ID);
    glBindTexture(target, ID);

   
    glTexImage2D(target, 0, desc.internalFormat, width, height, 0, desc.format, desc.type, data);

  
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, desc.minFilter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, desc.magFilter);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, desc.wrapS);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, desc.wrapT);

    if (desc.generateMipmaps) {
        glGenerateMipmap(target);
    }

    glBindTexture(target, 0);

  //  std::cout << "Texture created, ID: " << ID << ", size: " << width << "x" << height << std::endl;
}


void Texture::Bind(int slot)  {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(target, ID);
}

void Texture::Unbind() const {
    glBindTexture(target, 0);
}




