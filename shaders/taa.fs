#version 330 core

out vec4 FragColor; // Final output color

in vec2 TexCoords;          // Input texture coordinates
in vec2 MotionVector;       // Input motion vector
in float Depth;             // Input depth value (new)

uniform sampler2D screenTexture;        // Current frame texture
uniform sampler2D historyTexture;       // Previous frame texture
uniform sampler2D depthTexture;         // Depth texture for the scene
uniform vec2 inverseScreenSize;         // Inverse of the screen size

// Blend factor for the current frame vs history frame
uniform float baseBlendFactor; // Base blend factor that can be adjusted
uniform float depthThreshold;   // Threshold to determine if we should blend

void main()
{
    // Calculate the adjusted texture coordinate based on motion vectors
    vec2 currentCoord = TexCoords + MotionVector * inverseScreenSize; // Adjust for motion
    vec4 currentColor = texture(screenTexture, currentCoord);
    vec4 historyColor = texture(historyTexture, TexCoords); // Sample from history

    // Sample the depth for the current fragment
    float currentDepth = Depth; // Use the depth passed from the vertex shader
    float historyDepth = texture(depthTexture, TexCoords).r; // Use red channel for history depth

    // Calculate depth difference
    float depthDifference = abs(currentDepth - historyDepth);

    // Determine blending factor based on depth difference
    float blendFactor = baseBlendFactor; // Start with the base blend factor

    // Adjust the blend factor based on depth differences
    if (depthDifference > depthThreshold) {
        blendFactor = 1.0; // Use current frame only if the depth difference is too large
    } else {
        // Optionally smooth the blend factor
        blendFactor = mix(baseBlendFactor, 1.0, smoothstep(0.0, depthThreshold, depthDifference));
    }

    // Blend the colors using the calculated blend factor
    FragColor = mix(historyColor, currentColor, blendFactor); // Adjust the mix factor based on your needs
}