#version 330 core
out vec4 FragColor;

in vec2 vUV;
in vec3 vNormal;
in vec3 vCrntPos;
in vec3 vColor;
in vec4 fragPosLight;
uniform sampler2D tex0; // Albedo
uniform sampler2D tex1; // Specular/roughness/metallic etc.
uniform sampler2D shadowMap; // TO-DO: shadow atlas
uniform int shadowCasterCount;
uniform vec3 cameraPos;


struct GPULight {
    int type;         // 0 = point, 1 = directional, 2 = spot
    float intensity;  // light strength
    vec2 pad0;        // padding to make 16 bytes
    vec3 color;
    float pad1;
    vec3 position;
    float pad2;
    vec3 direction;
    float pad3;
    float innerCone;
    float outerCone;
    float pad4;
    int shadowMapIndex; // index into shadow atlas for this light's shadow map
     mat4 lightSpaceMatrix;
};

layout(std140) uniform LightBlock {
    GPULight lights[32]; // max 32 lights
    int lightCount;
    vec3 pad5;           // pad to 16 bytes
};



vec3 calcPointLight(GPULight light, vec3 normal, vec3 viewDir, vec3 albedo, float specularMap) {

    vec3 lightVec = light.position - vCrntPos;
    float distance = length(lightVec);
    vec3 L = normalize(lightVec);
    
    float attenuation = 1.0 / (0.01 * distance * distance + 0.001 * distance + 1.0);
    
   
    float diffuse = max(dot(normal, L), 0.0);
    
   
    vec3 H = normalize(L + viewDir);
    float spec = pow(max(dot(normal, H), 0.0), 32.0) * specularMap; // Higher exponent for Blinn-Phong
    
    return (albedo * diffuse + spec) * light.color * light.intensity * attenuation;
}



float calcShadowDirectional(GPULight light, vec3 normal)
{
    if (light.shadowMapIndex < 0) return 0.0;

    vec4 fragPosLight = light.lightSpaceMatrix * vec4(vCrntPos, 1.0);
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 0.0;

    vec2 atlasUV = vec2(
        (projCoords.x + float(light.shadowMapIndex)) / float(shadowCasterCount),
        projCoords.y
    );

    float currentDepth = projCoords.z;
    float bias = max(0.004 * (1.0 - dot(normalize(vNormal), normalize(-light.direction))), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    texelSize.x /= float(shadowCasterCount);

    for (int y = -2; y <= 2; y++)
    for (int x = -2; x <= 2; x++)
    {
        float closestDepth = texture(shadowMap, atlasUV + vec2(x, y) * texelSize).r;
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    return shadow / 25.0;
}

float calcShadowSpot(GPULight light, vec3 normal)
{
    if (light.shadowMapIndex < 0) return 0.0;

    vec4 fragPosLight = light.lightSpaceMatrix * vec4(vCrntPos, 1.0);
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 0.0;

    // clip corners outside the cone circle
    vec2 centered = projCoords.xy * 2.0 - 1.0;
    if (dot(centered, centered) > 1.0) return 0.0;

    vec2 atlasUV = vec2(
        (projCoords.x + float(light.shadowMapIndex)) / float(shadowCasterCount),
        projCoords.y
    );

    float currentDepth = projCoords.z;
    float bias = max(0.004 * (1.0 - dot(normalize(vNormal), normalize(-light.direction))), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    texelSize.x /= float(shadowCasterCount);

    for (int y = -2; y <= 2; y++)
    for (int x = -2; x <= 2; x++)
    {
        float closestDepth = texture(shadowMap, atlasUV + vec2(x, y) * texelSize).r;
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    return shadow / 25.0;
}

vec3 calcDirectionalLight(GPULight light, vec3 normal, vec3 viewDir, vec3 albedo, float specularMap)
{
    vec3 L = normalize(-light.direction);
    float diffuse = max(dot(normal, L), 0.0);
    vec3 H = normalize(L + viewDir);
    float spec = pow(max(dot(normal, H), 0.0), 32.0) * specularMap;

    float shadow = calcShadowDirectional(light, normal);

    return (albedo * diffuse + spec) * (1.0 - shadow) * light.color * light.intensity;
}

vec3 calcSpotLight(GPULight light, vec3 normal, vec3 viewDir, vec3 albedo, float specularMap)
{
    vec3 lightVec = light.position - vCrntPos;
    float distance = length(lightVec);
    vec3 L = normalize(lightVec);

    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.outerCone - light.innerCone;
    float intensity = clamp((theta - light.innerCone) / epsilon, 0.0, 1.0);
    float attenuation = 1.0 / (0.01 * distance * distance + 0.001 * distance + 1.0);

    float diffuse = max(dot(normal, L), 0.0);
    vec3 H = normalize(L + viewDir);
    float spec = pow(max(dot(normal, H), 0.0), 32.0) * specularMap;

    float shadow = calcShadowSpot(light, normal);

    return (albedo * diffuse + spec) * (1.0 - shadow) * light.color * light.intensity * attenuation * intensity;
}
void main() {
    vec3 normal = normalize(vNormal);
    vec3 viewDir = normalize(cameraPos - vCrntPos);
    
    vec3 albedo = texture(tex0, vUV).rgb;
    float specMap = texture(tex1, vUV).r;
    
    vec3 ambient = albedo * 0.10; 
    vec3 result = ambient;
    
    for (int i = 0; i < lightCount; i++) {
        GPULight light = lights[i];
        
        if (light.type == 0) {
            result += calcPointLight(light, normal, viewDir, albedo, specMap);
        } else if (light.type == 1) {
            result += calcDirectionalLight(light, normal, viewDir, albedo, specMap);
        } else if (light.type == 2) {
            result += calcSpotLight(light, normal, viewDir, albedo, specMap);
        }
    }
    
   
  FragColor = vec4(result, 1.0);

//FragColor = vec4(texture(shadowMap, vUV).rrr, 1.0);
}