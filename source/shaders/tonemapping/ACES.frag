#version 420 core

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D uBaseInput;

uniform float exposure = 0.08;

vec3 ACESFilm(vec3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;

    return clamp(
        (x * (a * x + b)) /
        (x * (c * x + d) + e),
        0.0,
        1.0
    );
}

void main()
{
    vec3 hdr =
        texture(uBaseInput, texCoords).rgb;

    hdr *= pow(2.0, exposure);

    vec3 mapped =
        ACESFilm(hdr);

    FragColor =
        vec4(mapped, 1.0);
}