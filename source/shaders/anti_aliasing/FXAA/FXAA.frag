#version 330 core
out vec4 FragColor;
in vec2 vUV;
uniform sampler2D screenTex;

uniform float threshold = 0.1;
uniform float blendStrength = 0.5;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(screenTex, 0));
    vec3 center = texture(screenTex, vUV).rgb;
    vec3 right  = texture(screenTex, vUV + vec2(texelSize.x, 0.0)).rgb;
    vec3 left   = texture(screenTex, vUV - vec2(texelSize.x, 0.0)).rgb;
    vec3 up     = texture(screenTex, vUV + vec2(0.0, texelSize.y)).rgb;
    vec3 down   = texture(screenTex, vUV - vec2(0.0, texelSize.y)).rgb;

    float lumaCenter = dot(center, vec3(0.299, 0.587, 0.114));
    float lumaRight  = dot(right, vec3(0.299, 0.587, 0.114));
    float lumaLeft   = dot(left, vec3(0.299, 0.587, 0.114));
    float lumaUp     = dot(up, vec3(0.299, 0.587, 0.114));
    float lumaDown   = dot(down, vec3(0.299, 0.587, 0.114));
    float lumaMax    = max(lumaCenter, max(lumaRight, max(lumaLeft, max(lumaUp, lumaDown))));
    float lumaMin    = min(lumaCenter, min(lumaRight, min(lumaLeft, min(lumaUp, lumaDown))));

    if (lumaMax - lumaMin <= threshold) {
        FragColor    = vec4(center, 1.0);
    } else {
        float horizontal = abs(lumaUp + lumaDown - 2.0 * lumaCenter);
        float vertical   = abs(lumaRight + lumaLeft - 2.0 * lumaCenter);
        vec3 neighbor    = horizontal > vertical ? mix(up, down, 0.5) : mix(left, right, 0.5);
        FragColor        = vec4(mix(center, neighbor, blendStrength), 1.0);
    }
}