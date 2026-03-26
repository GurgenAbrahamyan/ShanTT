#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in float aTangentW;
layout(location = 6) in mat4 instanceModel;

out vec3 vColor;
out vec2 vUV;
out vec3 vNormal;
out vec3 vCrntPos;
out mat3 TBN;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = instanceModel * vec4(aPos, 1.0);
    vCrntPos = worldPos.xyz;

    mat3 normalMatrix = transpose(inverse(mat3(instanceModel)));

    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    T = normalize(T - dot(T, N) * N); // Gram-Schmidt
    vec3 B = cross(N, T) * aTangentW; // handedness

    TBN = mat3(T, B, N);
    vNormal = N;
    vColor = aColor;
    vUV = aUV;
    gl_Position = projection * view * worldPos;
}