#version 330 core

layout(location = 0) in vec2 aPos; // Position
layout(location = 1) in vec2 aTexCoord; // Texture coordinates

out vec2 TexCoord;

void main() {
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos, 0.0, 1.0);
}