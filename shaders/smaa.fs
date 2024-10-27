#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D colorTexture; // Current frame texture
uniform sampler2D historyTexture; // Previous frame texture
uniform sampler2D depthTexture; // Depth texture

uniform float baseBlendFactor;
uniform float depthThreshold;

void main() {
    vec3 colorCurrent = texture(colorTexture, TexCoord).rgb;
    vec3 colorHistory = texture(historyTexture, TexCoord).rgb;
    float depthCurrent = texture(depthTexture, TexCoord).r;

    float blendFactor = depthCurrent < depthThreshold ? baseBlendFactor : 1.0;

    FragColor = vec4(mix(colorCurrent, colorHistory, blendFactor), 1.0);
}