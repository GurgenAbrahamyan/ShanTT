#version 420 core
layout (location = 0) out vec4 gAlbedo;
layout (location = 1) out vec4 gPosition;
layout (location = 2) out vec4 gNormal;
layout (location = 3) out vec4 gARM;
layout (location = 4) out vec4 gEmissive;


in vec2 vUV;
in vec3 vNormal;
in vec3 vCrntPos;
in vec3 vColor;
in mat3 TBN;

uniform sampler2D albedoMap;   
uniform sampler2D armMap;     
uniform sampler2D normalMap;   
uniform sampler2D emissiveMap;


uniform vec4  baseColorFactor;
uniform float metallicFactor;
uniform float roughnessFactor;
uniform float aoFactor;
uniform vec3  emissiveFactor;




void main()
{
    
    vec3 normalSample = texture(normalMap, vUV).rgb * 2.0 - 1.0;
    vec3 normal = normalize(TBN * normalSample);

    
    vec3 albedo = texture(albedoMap, vUV).rgb * baseColorFactor.rgb;

    vec3 arm = texture(armMap, vUV).rgb;
    float ao        = arm.r * aoFactor;
    float roughness = arm.g * roughnessFactor;
    float metallic  = arm.b * metallicFactor;

    vec3 emissive = texture(emissiveMap, vUV).rgb * emissiveFactor;

 
    gPosition = vec4(vCrntPos, 1.0);                 // TEMP
    gNormal   = vec4(normal, 1.0);
    gAlbedo   = vec4(albedo, 1.0);
    gARM      = vec4(ao, roughness, metallic, 1.0);
    gEmissive = vec4(emissive, 1.0);
}

