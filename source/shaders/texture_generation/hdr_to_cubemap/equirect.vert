#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;

out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;
void main() {
    WorldPos = aPos;
    gl_Position = projection * view * vec4(WorldPos, 1.0);
}