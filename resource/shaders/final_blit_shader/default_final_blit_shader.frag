#version 420 core

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform sampler2D bloomTexture;

uniform float exposure = 0.0;
uniform float bloomStrength = 0.08;

uniform bool isBloom = false;

vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    vec3 hdr = texture(sceneTexture, texCoords).rgb;
    vec3 bloom = isBloom ? texture(bloomTexture, texCoords).rgb : vec3(0.0);
    // energy-controlled combine
    vec3 color = hdr + bloom * bloomStrength;
    color *= pow(2.0, exposure);
    // ACES tonemap
    vec3 mapped = ACESFilm(color);



    // gamma correction
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}