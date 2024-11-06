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

// Utility function to generate a random value between 0.0 and 1.0
float Hash(vec2 p) {
    p = fract(p * 0.1031); // Prime number multiplier for better randomness
    p += dot(p, p + 33.333);
    return fract(p.x * p.y);
}

// Generate random direction in the hemisphere based on normal
vec3 RandomHemisphereDirection(vec3 normal, float randomX, float randomY) {
    float phi = 2.0 * 3.14159265359 * randomX;
    float theta = acos(2.0 * randomY - 1.0);  // In range [0, pi]
    
    vec3 tangent1 = normalize(cross(abs(normal.x) > 0.1 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0), normal));
    vec3 tangent2 = cross(normal, tangent1);
    
    vec3 randomDir = sin(theta) * cos(phi) * tangent1 + sin(theta) * sin(phi) * tangent2 + cos(theta) * normal;
    return normalize(randomDir);
}

// Check if the sampled point is occluded by geometry
bool IsOccluded(vec3 samplePosition, vec2 uv) {
    float sampleDepth = texture(depthTexture, uv).r;
    return (length(samplePosition - cameraPosition) > sampleDepth);
}

// SSGI function to calculate diffuse illumination
vec3 SSGI(vec2 uv, vec3 normal, vec3 albedo) {
    vec3 color = vec3(0.0);
    for (int i = 0; i < sampleCount; i++) {
        // Generate random direction in hemisphere
        vec2 randomUV = TexCoord + vec2(Hash(TexCoord + float(i)), Hash(TexCoord - float(i)));
        float randomX = Hash(randomUV);
        float randomY = Hash(randomUV + vec2(1.0, 0.0));
        vec3 randomDirection = RandomHemisphereDirection(normal, randomX, randomY);

        // Sample position and normal from the neighboring pixel
        vec3 samplePosition = GetWorldPosition(uv + randomDirection.xy * radius / screenSize);
        vec3 sampleNormal = texture(normalTexture, uv + randomDirection.xy * radius / screenSize).xyz;

        // Check if the sample is occluded
        if (IsOccluded(samplePosition, uv)) {
            continue;
        }

        // Calculate the angle between the normals (dot product)
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