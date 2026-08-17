#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in float aTangentW;
layout(location = 6) in mat4 instanceModel;

out vec2 vUV;
out vec3 vNormal;
out vec3 vCrntPos;
out float vLinearDepth;   
out mat3 TBN;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos  = instanceModel * vec4(aPos, 1.0);
    vCrntPos       = worldPos.xyz;

    vec4 viewPos   = view * worldPos;
    vLinearDepth   = -viewPos.z;              // positive depth in view space

    mat3 normalMatrix = transpose(inverse(mat3(instanceModel)));
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * aTangentW;
    TBN = mat3(T, B, N);
    vNormal = N;
    vUV = aUV;
    gl_Position = projection * view * worldPos;
}