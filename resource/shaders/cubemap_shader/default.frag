#version 330 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main()
{    
    float specular = 0.5f;
    FragColor = texture(skybox, texCoords)*specular;
}