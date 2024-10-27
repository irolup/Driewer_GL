#version 330 core

out vec4 FragColor;

in float Height;
in vec2 TexCoord; // Receive UV coordinates from the vertex shader

uniform sampler2D texture_diffuse; // Add a texture sampler

void main()
{
    float h = (Height + 16) / 32.0f; // Shift and scale the height into a grayscale value
    
    // Sample the texture using UV coordinates
    vec4 textureColor = texture(texture_diffuse, TexCoord); 
    
    // Combine height information with texture color
    FragColor = vec4(h * textureColor.rgb, 1.0); // Modify the output color using height and texture
}