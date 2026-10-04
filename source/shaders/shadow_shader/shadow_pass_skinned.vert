#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in float aTangentW;

layout(location = 6) in mat4 instanceModel;

layout(location = 10) in uvec4 aJoints;
layout(location = 11) in vec4 aWeights;

layout(location = 12) in uint paletteOffset;
layout(location = 13) in float enableSkining;

uniform mat4 lightSpaceMatrix;

layout(std430, binding = 0) readonly buffer SkinMatrices
{
    mat4 jointMatrices[];
};

void main()
{
    vec4 skinnedPos;

    if (enableSkining > 0.5)
    {
        mat4 skinMatrix =
              aWeights.x *
                  jointMatrices[paletteOffset + aJoints.x]
            + aWeights.y *
                  jointMatrices[paletteOffset + aJoints.y]
            + aWeights.z *
                  jointMatrices[paletteOffset + aJoints.z]
            + aWeights.w *
                  jointMatrices[paletteOffset + aJoints.w];

        skinnedPos =
            skinMatrix * vec4(aPos, 1.0);
    }
    else
    {
        skinnedPos = vec4(aPos, 1.0);
    }

    gl_Position =
        lightSpaceMatrix *
        (instanceModel * skinnedPos);
}