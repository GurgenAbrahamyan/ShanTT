#version 420 core
out vec4 FragColor;
in vec2 vUV;

uniform sampler2D gAlbedo;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gARM;
uniform sampler2D gEmissive;
uniform sampler2D shadowMap;

uniform bool shadowsEnabled;
uniform vec3 cameraPos;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform float envIntensity;

struct GPULight {
    int   type;           
    float intensity;
    vec2  pad0;
    vec3  color;
    float pad1;
    vec3  position;
    float pad2;
    vec3  direction;
    float pad3;
    float innerCone, outerCone;
    int   shadowIndex;
    int   pad5;
};

struct ShadowData {
    vec2 minUv;
    vec2 maxUv;
    mat4 lightMatrix;
};

layout(std140) uniform LightBlock {
    GPULight lights[32]; 
    int lightCount;
    vec3 pad5;        
};
const float PI = 3.14159265359;


vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return  F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - max(cosTheta, 0.0), 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = max(denom, 0.0001);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    //For direct Lighting
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.001);
    float NdotL = max(dot(N, L), 0.001);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

layout(std140, binding = 2) uniform LightMatrices {
    ShadowData shadowInfo[32];
};

const vec2 poissonDisk[16] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2( 0.94558609, -0.76890725),
    vec2(-0.094184101,-0.92938870),
    vec2( 0.34495938,  0.29387760),
    vec2(-0.91588581,  0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543,  0.27676845),
    vec2( 0.97484398,  0.75648379),
    vec2( 0.44323325, -0.97511554),
    vec2( 0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2( 0.79197514,  0.19090188),
    vec2(-0.24188840,  0.99706507),
    vec2(-0.81409955,  0.91437590),
    vec2( 0.19984126,  0.78641367),
    vec2( 0.14383161, -0.14100790)
);

float randomAngle(vec3 seed) {
    return fract(sin(dot(seed, vec3(12.9898,78.233,45.5432)))*43758.5453)*6.2832;
}

float pcf(sampler2D map, vec2 atlasUV, float currentDepth, float bias, vec2 tileSize, float spreadPixels) {
    vec2 texelSize = tileSize / vec2(textureSize(map,0));
    float angle    = randomAngle(vec3(atlasUV,currentDepth));
    float s        = sin(angle), c = cos(angle);
    mat2 rot       = mat2(c,-s,s,c);
    float shadow   = 0.0;

    for(int i=0;i<16;i++){
        vec2 offset = rot * poissonDisk[i] * texelSize * spreadPixels;
        float closest = texture(map, atlasUV + offset).r;
        shadow += (currentDepth - bias > closest) ? 1.0 : 0.0;
    }
    return shadow/16.0;
}


float calcShadowPoint(GPULight light, vec3 normal, vec3 fragPos) {

    if(light.shadowIndex<0) 
         return 0.0;

    vec3 L    = fragPos - light.position;
    vec3 absL = abs(L);
    int face;

    if(absL.x >= absL.y && absL.x >= absL.z) 
         face=(L.x >= 0 ? 0 : 1);


    else if(absL.y>=absL.x && absL.y>=absL.z) 
         face=(L.y>=0?2:3);
    else 
         face=(L.z>=0?4:5);

    int idx = light.shadowIndex + face;
    vec4 fp = shadowInfo[idx].lightMatrix * vec4(fragPos,1.0);
    vec3 projCoords = fp.xyz/fp.w * 0.5+0.5;
    if(projCoords.z>1.0) return 0.0;

    vec2 tileSize = shadowInfo[idx].maxUv - shadowInfo[idx].minUv;
    vec2 atlasUV  = shadowInfo[idx].minUv + projCoords.xy*tileSize;
    vec3 Lnorm    = normalize(light.position - fragPos);
    float bias    = max(0.004*(1.0 - dot(normalize(normal), Lnorm)),0.0009);
    return pcf(shadowMap, atlasUV, projCoords.z, bias, tileSize, 10);
}

float calcShadowDirectional(GPULight light, vec3 normal, vec3 fragPos) {

    if(light.shadowIndex < 0) 
        return 0.0;

    int idx       = light.shadowIndex;

    vec4 fp       = shadowInfo[idx].lightMatrix * vec4(fragPos,1.0);
    vec3 projCoords = fp.xyz / fp.w * 0.5 + 0.5;

    if(projCoords.z > 1.0) 
         return 0.0;

    vec2 tileSize = shadowInfo[idx].maxUv - shadowInfo[idx].minUv;
    vec2 atlasUV  = shadowInfo[idx].minUv + projCoords.xy * tileSize;

    float bias    = max(0.004 * (1.0 - dot(normalize(normal), normalize( -light.direction ))), 0.0005);
    return pcf(shadowMap, atlasUV, projCoords.z, bias, tileSize, 17);
}

float calcShadowSpot(GPULight light, vec3 normal, vec3 fragPos) {

    if(light.shadowIndex < 0) 
        return 0.0;

    int idx = light.shadowIndex;
    vec4 fp = shadowInfo[idx].lightMatrix * vec4(fragPos,1.0);

    vec3 projCoords = fp.xyz / fp.w * 0.5+0.5;
    if(projCoords.z > 1.0) 
        return 0.0;

    vec2 centered = projCoords.xy * 2.0-1.0;
    if(dot(centered, centered)>1.0) return 0.0;

    vec2 tileSize = shadowInfo[idx].maxUv - shadowInfo[idx].minUv;
    vec2 atlasUV = shadowInfo[idx].minUv + projCoords.xy * tileSize;

    float bias = max(0.004 * (1.0 - dot(normalize(normal), normalize( -light.direction ) ) ), 0.0005);
    return pcf(shadowMap, atlasUV, projCoords.z, bias, tileSize, 35);
}


vec3 cookTorrance(vec3 L, vec3 N, vec3 V, vec3 albedo,
                  float roughness, float metallic,
                  vec3 lightColor, float lightIntensity, float shadow)
{
    vec3 H = normalize(V + L);

    // F0: 0.04 for dielectrics, lerp toward albedo for metals
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    float NdotL = max(dot(N, L), 0.001);
    float NdotV = max(dot(N, V), 0.001);

    // Cook-Torrance specular terms
    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3  F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3  num   = D * F * G;
    float denom = 4.0 * NdotV * NdotL + 0.0001; // epsilon to avoid /0
    vec3  spec  = num / denom;

   
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic); // metals have no diffuse

    vec3 diffuse = kD * albedo / PI;

    return (diffuse + spec) * NdotL * lightColor * lightIntensity * (1.0 - shadow);
}
// Light calculations
vec3 calcPointLight(GPULight light, vec3 N, vec3 V,
                    vec3 albedo, float roughness, float metallic, vec3 fragPos)
{
    vec3 Lvec = light.position - fragPos;
    float dist = length(Lvec);
    vec3  L    = normalize(Lvec);
    float attenuation = 1.0 / dist*dist;

    float shadow = shadowsEnabled ? calcShadowPoint(light, N, fragPos) : 0.0;

    return cookTorrance(L, N, V, albedo, roughness, metallic,
                        light.color, light.intensity * attenuation, shadow);
}

vec3 calcDirectionalLight(GPULight light, vec3 N, vec3 V,
                          vec3 albedo, float roughness, float metallic, vec3 fragPos)
{
    vec3 L = normalize(-light.direction);
    float shadow = shadowsEnabled ? calcShadowDirectional(light, N, fragPos) : 0.0;

    return cookTorrance(L, N, V, albedo, roughness, metallic,
                        light.color, light.intensity, shadow);
}

vec3 calcSpotLight(GPULight light, vec3 N, vec3 V,
                   vec3 albedo, float roughness, float metallic, vec3 fragPos)
{
    vec3  Lvec = light.position - fragPos;
    float dist = length(Lvec);
    vec3  L    = normalize(Lvec);

    float theta   = dot(L, normalize(-light.direction));
    float epsilon  = light.outerCone - light.innerCone;
    float spotMask = clamp((theta - light.innerCone) / epsilon, 0.0, 1.0);
    float attenuation = spotMask / (0.01*dist*dist + 0.001*dist + 1.0);

    float shadow = shadowsEnabled ? calcShadowSpot(light, N, fragPos) : 0.0;

    return cookTorrance(L, N, V, albedo, roughness, metallic,
                        light.color, light.intensity * attenuation, shadow);
}

void main() {
    vec3 fragPos = texture(gPosition, vUV).xyz;

    vec4 normalSample = texture(gNormal, vUV);
    
    if(normalSample.a < 0.5) {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    vec3 normal = normalize(normalSample.xyz);
    vec3  albedo   = texture(gAlbedo, vUV).rgb; 
    vec3  arm      = texture(gARM, vUV).rgb;
    vec3  emissive = texture(gEmissive, vUV).rgb;

    float ao        = arm.r;
    float roughness = arm.g;
    
    roughness = max(roughness, 0.04);
    float metallic  = arm.b;

    vec3 V = normalize(cameraPos - fragPos);
 
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    
    vec3 fresnel = fresnelSchlickRoughness(max(dot(normal, V), 0.0), F0, roughness);
    vec3 kS = fresnel;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, normal).rgb;
   


    vec3 R = reflect(-V, normal);
    R = normalize(R);
    float mipLevel =  roughness * 9.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, mipLevel).rgb;
    vec2 brdfSample = texture(brdfLUT, vec2(max(dot(normal,V),0.001), roughness)).rg;
    vec3 diffuseIBL = irradiance * albedo * envIntensity;
    vec3 specularIBL = prefilteredColor * (fresnel * brdfSample.x + brdfSample.y) * envIntensity;

    vec3 ambient = (kD * diffuseIBL + specularIBL) * ao;
    vec3 result = ambient;
 

     for (int i = 0; i < lightCount; i++) {
        GPULight light = lights[i];
        if      (light.type == 0) result += calcPointLight      (light, normal, V, albedo, roughness, metallic, fragPos);
        else if (light.type == 1) result += calcDirectionalLight (light, normal, V, albedo, roughness, metallic, fragPos);
        else if (light.type == 2) result += calcSpotLight        (light, normal, V, albedo, roughness, metallic, fragPos);
    }

    

    result += emissive;


   
    FragColor = vec4(result, 1.0);
}