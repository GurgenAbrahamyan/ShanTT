#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in float aTangentW;
layout(location = 6) in mat4 instanceModel;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = lightSpaceMatrix *( instanceModel *vec4(aPos, 1.0));
}