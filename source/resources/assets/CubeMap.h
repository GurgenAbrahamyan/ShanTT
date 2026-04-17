#pragma once
#include <glad/glad.h>
#include "../../render/backend/containers/VAO.h"
#include "../../render/backend/containers/VBO.h"


class CubeMap {
public:
   

    void bind() const;
    void unbind() const;
    void bindEnvTexture(int slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, EnvTexID);
    }
    void bindIrrTexture(int slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, IrrTexID);
    }
    void bindPreFilterTexture(int slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, PreFilterTexId);
    }
    void setEnvTexture(GLuint textureID) { EnvTexID = textureID; }
    void setIrrTexture(GLuint textureID) { IrrTexID = textureID; }
    void setPreFilterTexture(GLuint textureID) { PreFilterTexId = textureID; }

	GLuint getEnvTextureID() const { return EnvTexID; }
	GLuint getIrrTextureID() const { return IrrTexID; }
	GLuint getPreFilterTextureID() const { return PreFilterTexId; }
    CubeMap();
    ~CubeMap();

	
private:
    VAO* VAO1;
    VBO* VBO1;

    GLuint PreFilterTexId = 0;
    GLuint EnvTexID = 0;
    GLuint IrrTexID = 0;
};
