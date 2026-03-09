#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Vector2.h"
#include "../../math_custom/Mat4.h"


static std::string get_file_contents(const char* filename)
{
    std::ifstream in(filename, std::ios::binary);
    if (!in) throw std::runtime_error("Failed to open file: " + std::string(filename));

    std::ostringstream contents;
    contents << in.rdbuf();
    return contents.str();
}

Shader::Shader(const char* vertexFile, const char* fragmentFile, const char* geometryFile)
{
    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);
    std::string geometryCode = get_file_contents(geometryFile);

    GLuint vertexShader = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
	std::cout << "Vertex shader compiled successfully: " << vertexFile << "\n";
    GLuint fragmentShader = compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);
	std::cout << "Fragment shader compiled successfully: " << fragmentFile << "\n";
//	GLuint geometryShader = compileShader(geometryCode.c_str(), GL_GEOMETRY_SHADER);

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
//	glAttachShader(ID, geometryShader);
    glLinkProgram(ID);

    checkLinkErrors(ID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::Activate() const {
    glUseProgram(ID);
}

GLuint Shader::compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "Shader compilation error: " + std::string(infoLog);
        throw std::runtime_error("Shader compilation error: " + std::string(infoLog));
    }

    return shader;
}

void Shader::checkLinkErrors(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        throw std::runtime_error("Shader program linking error: " + std::string(infoLog));
    }
}

void Shader::setFloat(const char* name, float value) {
    glUniform1f(glGetUniformLocation(ID, name), value);
}
void Shader::setInt(const char* name, int value) {
    GLuint texUni = glGetUniformLocation(ID, name);
    
    Activate();

    glUniform1i(texUni, value);
   
}
void Shader::setVec2(const char* name, Vector2 value) {
    glUniform2f(glGetUniformLocation(ID, name), value.x, value.y);
}
void Shader::setVec3(const char* name, Vector3 value) {
    glUniform3f(glGetUniformLocation(ID, name), value.x, value.y, value.z);
}

void Shader::setVec4(const char* name, Vector3 value, float t) {
    glUniform4f(glGetUniformLocation(ID, name), value.x, value.y, value.z, t);
}
void Shader::setMat4(const char* name, const Mat4 mat) {

    GLint loc = glGetUniformLocation(ID, name);
    if (loc == -1) return;

    glUniformMatrix4fv(loc, 1, GL_FALSE, mat.data);
}


