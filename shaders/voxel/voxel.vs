#version 330

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

out VS_OUT {
    vec3 worldPosition;
    vec3 normal;
    vec2 texCoord;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 worldPos = model * vec4(inPosition, 1.0);
    vs_out.worldPosition = worldPos.xyz;
    vs_out.normal = mat3(transpose(inverse(model))) * inNormal;
    vs_out.texCoord = inTexCoord;

    gl_Position = worldPos; // Pass-through for geometry shader
}