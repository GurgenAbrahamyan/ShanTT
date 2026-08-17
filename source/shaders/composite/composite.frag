#version 420 core

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D uBaseInput;
uniform sampler2D uBloom;

uniform float bloomStrength = 0.08;

void main()
{
    vec3 hdr =
        texture(uBaseInput, texCoords).rgb;

    vec3 bloom =
        texture(uBloom, texCoords).rgb;

    vec3 color =
        hdr + bloom * bloomStrength;

    FragColor =
        vec4(color, 1.0);
}