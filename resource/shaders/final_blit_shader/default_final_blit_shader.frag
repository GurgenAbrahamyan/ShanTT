#version 420 core

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform sampler2D bloomTexture;

uniform float exposure = 1.0;
uniform float bloomStrength = 0.08;

uniform bool isBloom = false;

void main()
{
    vec3 hdr = texture(sceneTexture, texCoords).rgb;
    vec3 bloom = isBloom ? texture(bloomTexture, texCoords).rgb : vec3(0.0);
    // energy-controlled combine
    vec3 color = hdr + bloom * bloomStrength;

    // exposure tonemap
    vec3 mapped = vec3(1.0) - exp(-color * exposure);

    // gamma correction
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}