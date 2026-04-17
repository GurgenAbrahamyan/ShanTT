#version 420 core

out vec4 FragColor;
in vec2 texCoords;
layout( binding= 0) uniform sampler2D screenTexture;
layout( binding= 1) uniform sampler2D bloomTexture;
uniform float exposure; 
uniform bool isBloom = true;

void main()
{
    vec3 hdrColor = texture(screenTexture, texCoords).rgb;
   
    vec3 combined = hdrColor;
    if(isBloom){
     vec3 bloom    = texture(bloomTexture, texCoords).rgb;
     combined += bloom*0.05;
    
    }
    vec3 mapped   = vec3(1.0) - exp(-combined * exposure);
    mapped = pow(mapped, vec3(1.0/2.2)); // gamma correction
    FragColor = vec4(mapped, 1.0);
}