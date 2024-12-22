#version 460

// Input from the geometry shader
in GS_OUT {
    vec3 worldPosition;
    vec3 normal;
    vec2 texCoord;
} fs_in;

// Base texture and structured buffer
uniform sampler2D texture_diffuse;
layout(binding = 0, r32ui) uniform uimageBuffer voxelGrid;

// Voxel grid parameters
uniform vec3 voxelGridCenter;  // Center of the voxel grid
uniform float voxelGridSize;   // Size of the voxel grid
uniform int voxelResolution;   // Resolution of the voxel grid (number of voxels per axis)

// Light data
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};
uniform Light lights[16];
uniform int numLights;

// Output color for debugging visualization
out vec4 fragColor;

// Function to compute diffuse lighting
vec3 computeDiffuseLighting(vec3 position, vec3 normal) {
    vec3 diffuseColor = vec3(0.0);
    for (int i = 0; i < numLights; ++i) {
        vec3 lightDir = normalize(lights[i].position - position);
        float diff = max(dot(normal, lightDir), 0.0);
        diffuseColor += diff * lights[i].color * lights[i].intensity;
    }
    return diffuseColor;
}

// Function to pack HDR color into a 32-bit unsigned integer
uint packColor(vec3 color, float emissiveMultiplier, bool isEmpty) {
    uint r = uint(clamp(color.r * 255.0, 0.0, 255.0));
    uint g = uint(clamp(color.g * 255.0, 0.0, 255.0));
    uint b = uint(clamp(color.b * 255.0, 0.0, 255.0));
    uint emissive = uint(clamp(emissiveMultiplier * 127.0, 0.0, 127.0));
    uint emptyFlag = isEmpty ? 0u : 1u;
    return (r << 24) | (g << 16) | (b << 8) | (emissive << 1) | emptyFlag;
}

// Main fragment shader
void main() {
    // Compute voxel grid coordinates
    vec3 voxelCoords = (fs_in.worldPosition - voxelGridCenter) / voxelGridSize + 0.5;
    voxelCoords *= float(voxelResolution);
    ivec3 voxelIndex = ivec3(floor(voxelCoords));

    // Check if within bounds
    if (any(lessThan(voxelIndex, ivec3(0))) || any(greaterThanEqual(voxelIndex, ivec3(voxelResolution)))) {
        return;
    }

    // Flatten the voxel index
    int flattenedIndex = voxelIndex.x + voxelIndex.y * voxelResolution + voxelIndex.z * voxelResolution * voxelResolution;

    // Sample the base texture
    vec3 baseColor = texture(texture_diffuse, fs_in.texCoord).rgb;

    // Compute lighting
    vec3 diffuseLighting = computeDiffuseLighting(fs_in.worldPosition, normalize(fs_in.normal));

    // Final voxel color
    vec3 voxelColor = baseColor * diffuseLighting;

    // Write to the voxel grid using atomic operations
    uint packedColor = packColor(voxelColor, 0.0, false);
    imageAtomicMax(voxelGrid, flattenedIndex, packedColor);

    // Debug visualization: output the voxel color
    //fragColor = vec4(voxelColor, 1.0);
    fragColor = vec4(baseColor, 1.0);
}