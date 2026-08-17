#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D uSceneTexture;
uniform sampler2D uDepthTexture;
uniform float uFocusDistance;
uniform float uFocalLength;
uniform float uAperture;
uniform float uBlurScale;
uniform float uNearPlane;
uniform float uFarPlane;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * uNearPlane * uFarPlane) /
           (uFarPlane + uNearPlane - z * (uFarPlane - uNearPlane));
}

float ComputeCoC(float vp)
{
    float f  = uFocalLength;
    float v0 = uFocusDistance;
    float d  = uAperture;

    if (abs(v0 - f) < 0.0001) return 0.0;
    if (vp < 0.0001) return 0.0;

    float coc = d * (f / (v0 - f)) * abs(1.0 - v0 / vp);

    
    coc = max(0.0, coc -0.005); //  TO-DO: custom values

    return coc;
}

const vec2 poissonDisk[24] = vec2[](
    vec2( 0.000,  0.000),
    vec2( 0.500,  0.000),
    vec2( 0.250,  0.433),
    vec2(-0.250,  0.433),
    vec2(-0.500,  0.000),
    vec2(-0.250, -0.433),
    vec2( 0.250, -0.433),
    vec2( 1.000,  0.000),
    vec2( 0.707,  0.707),
    vec2( 0.000,  1.000),
    vec2(-0.707,  0.707),
    vec2(-1.000,  0.000),
    vec2(-0.707, -0.707),
    vec2( 0.000, -1.000),
    vec2( 0.707, -0.707),
    vec2( 0.809,  0.588),
    vec2( 0.309,  0.951),
    vec2(-0.309,  0.951),
    vec2(-0.809,  0.588),
    vec2(-1.000,  0.000),
    vec2(-0.809, -0.588),
    vec2(-0.309, -0.951),
    vec2( 0.309, -0.951),
    vec2( 0.809, -0.588)
);

void main()
{
    float depthSample = texture(uDepthTexture, TexCoords).r;
    float vp = LinearizeDepth(depthSample);
    float coc = ComputeCoC(vp);
    float cocPixels = clamp(coc * uBlurScale, 0.0, 25.0);

    vec2 texelSize = 1.0 / vec2(textureSize(uSceneTexture, 0));
    vec3 color = vec3(0.0);
    for (int i = 0; i < 24; ++i)
        color += texture(uSceneTexture, TexCoords + poissonDisk[i] * cocPixels * texelSize).rgb;

    FragColor = vec4(color / 24.0, 1.0);
}