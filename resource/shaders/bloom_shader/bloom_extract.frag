#version 420 core
in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;

void main(){

     FragColor = texture(sceneTexture, texCoords);

    
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        FragColor = vec4(FragColor.rgb, 1.0);
    else
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    
}