#version 330 core

out vec4 FragColor;  // Output color to the framebuffer

// G-buffer textures
uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D albedoTexture;
uniform sampler2D depthTexture;

// Camera and screen space settings
uniform vec2 screenSize;
uniform vec3 cameraPosition;

// SSGI settings
uniform float radius;  // Max radius for light sampling
uniform int sampleCount;  // Number of samples for global illumination

in vec2 TexCoord;  // Texture coordinates from vertex shader

// Utility function to compute world space position from depth
vec3 GetWorldPosition(vec2 uv) {
    float depth = texture(depthTexture, uv).r;
    return (cameraPosition + vec3(uv * 2.0 - 1.0, depth));
}

// SSGI function to calculate diffuse illumination
vec3 SSGI(vec2 uv, vec3 normal, vec3 albedo) {
    vec3 color = vec3(0.0);
    for (int i = 0; i < sampleCount; i++) {
        // Sample random directions around the current pixel
        vec2 offset = vec2(rand(), rand()) * radius;
        vec3 samplePosition = GetWorldPosition(uv + offset / screenSize);
        vec3 sampleNormal = texture(normalTexture, uv + offset / screenSize).xyz;

        // Calculate the angle between normals (dot product)
        float NdotL = max(dot(normal, sampleNormal), 0.0);
        color += NdotL * albedo;  // Accumulate lighting
    }

    return color / float(sampleCount);
}

void main() {
    // Fetch texture data
    vec3 position = texture(positionTexture, TexCoord).xyz;
    vec3 normal = texture(normalTexture, TexCoord).xyz;
    vec3 albedo = texture(albedoTexture, TexCoord).rgb;

    // Normalize the normal
    normal = normalize(normal);

    // Compute the SSGI lighting
    vec3 ssgiColor = SSGI(TexCoord, normal, albedo);

    // Final color is the combination of SSGI and base albedo
    FragColor = vec4(ssgiColor, 1.0);
}