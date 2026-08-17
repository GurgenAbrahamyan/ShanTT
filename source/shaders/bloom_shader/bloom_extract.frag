#version 420 core
in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;

// fixed - soft knee, smooth falloff
void main() {
    vec3 color = texture(sceneTexture, texCoords).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    float threshold = 1.0;
    float knee = 0.3; // softness around threshold
    float soft = brightness - threshold + knee;
    soft = clamp(soft, 0.0, 2.0 * knee);
    soft = (soft * soft) / (4.0 * knee + 0.00001);

    float contribution = max(soft, brightness - threshold) / max(brightness, 0.00001);
    FragColor = vec4(color * contribution, 1.0);
}