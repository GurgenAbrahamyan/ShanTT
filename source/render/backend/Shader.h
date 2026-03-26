#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <string>

class Vector2;
class Vector3;
struct Vector4;
class Mat4;

class Shader {
public:
    GLuint ID;

    Shader(const char* vertexFile, const char* fragmentFile, const char* geometryFile);
    ~Shader();

    void Activate() const;
    void setFloat(const char* name, float value);
    void setInt(const char* name, int value);
	void setVec2(const char* name, Vector2 value);
    void setVec3(const char* name, Vector3 value);
    void setVec4(const char* name, Vector4 value);
    void setMat4(const char* name, Mat4 value);

private:
    GLuint compileShader(const char* source, GLenum type);
    void checkLinkErrors(GLuint program);
};

#endif
