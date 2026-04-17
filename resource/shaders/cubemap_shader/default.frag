out vec4 FragColor;

in vec3 localPos;
  
uniform samplerCube skybox;
uniform float envIntensity;
void main()
{
   vec3 envColor = textureLod(skybox, localPos, 1.2).rgb; 
    
   
  
    FragColor = vec4(envColor * envIntensity, 1.0);
}