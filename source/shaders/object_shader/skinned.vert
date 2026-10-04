#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in float aTangentW;

layout(location = 6) in mat4 instanceModel;

layout(location = 10) in uvec4 aJoints;
layout(location = 11) in vec4 aWeights;

layout(location = 12) in uint paletteOffset;
layout(location = 13) in float enableSkining;

out vec2 vUV;
out vec3 vNormal;
out vec3 vCrntPos;
out float vLinearDepth;
out mat3 TBN;

uniform mat4 view;
uniform mat4 projection;

layout(std430, binding = 0) readonly buffer SkinMatrices
{
    mat4 jointMatrices[];
};

void main(){

    vec4 skinnedPos;
    vec3 skinnedNormal;
    vec3 skinnedTangent;

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

        skinnedNormal =
            mat3(skinMatrix) * aNormal;

        skinnedTangent =
            mat3(skinMatrix) * aTangent;
    }
    else
    {
        skinnedPos      = vec4(aPos, 1.0);
        skinnedNormal   = aNormal;
        skinnedTangent  = aTangent;
    }

    vec4 worldPos =
        instanceModel * skinnedPos;

    vCrntPos =
        worldPos.xyz;

    vec4 viewPos =
        view * worldPos;

    vLinearDepth =
        -viewPos.z;


    // -------------------------------------------------
    // Normal / tangent → world space
    // -------------------------------------------------

    mat3 normalMatrix =
        transpose(
            inverse(
                mat3(instanceModel)
            )
        );

    vec3 N =
        normalize(
            normalMatrix *
            skinnedNormal
        );

    vec3 T =
        normalize(
            normalMatrix *
            skinnedTangent
        );

    // Re-orthogonalize tangent.
    T =
        normalize(
            T -
            dot(T, N) * N
        );

    vec3 B =
        cross(N, T) *
        aTangentW;

    TBN =
        mat3(T, B, N);

    vNormal =
        N;

    vUV =
        aUV;


    gl_Position =
        projection *
        view *
        worldPos;
}