#version 330 core

out vec4 FragColor; // Output color

in vec2 TexCoords; // Texture coordinates from vertex shader

uniform sampler2D texture1; // The texture to sample

void main()
{
    // Sample the texture using the texture coordinates
    FragColor = texture(texture1, TexCoords);
}