#version 330 core

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;

void main()
{
    vec3 color = texture(sceneTexture, texCoords).rgb;

    color = pow(max(color, vec3(0.0)), vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}