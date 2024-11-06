#version 330 core

layout (location = 0) in vec3 inPosition;  // Vertex position
layout (location = 1) in vec2 inTexCoord;  // Texture coordinates

out vec2 TexCoord;  // Pass texture coordinates to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Calculate the final position
    gl_Position = projection * view * model * vec4(inPosition, 1.0);
    TexCoord = inTexCoord;  // Pass texture coordinates to fragment shader
}