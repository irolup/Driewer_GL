#version 330 core

layout(location = 0) in vec3 aPos;            // Vertex position
layout(location = 1) in vec2 aTexCoords;      // Texture coordinates
layout(location = 2) in vec2 aMotionVectors;   // Motion vectors
layout(location = 3) in float aDepth;          // Depth value (new attribute)

out vec2 TexCoords;                           // Output texture coordinates
out vec2 MotionVector;                        // Output motion vector
out float Depth;                              // Output depth value

void main()
{
    TexCoords = aTexCoords;
    MotionVector = aMotionVectors;
    Depth = aDepth;                          // Set the depth output
    gl_Position = vec4(aPos, 1.0);          // Set the vertex position
}