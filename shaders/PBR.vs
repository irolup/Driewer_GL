#version 330 core

// Vertex attributes
layout(location = 0) in vec4 position;  // Vertex position
layout(location = 1) in vec4 normal;    // Vertex normal
layout(location = 2) in vec2 texcoord;  // Texture coordinates

// Outputs to the fragment shader
out vec3 surface_position;
out vec3 surface_normal;
out vec2 surface_texcoord;
out vec4 eyeSpaceVertexPos;
out vec3 interp_eyePos;
out vec2 varyingtexcoord;

// Uniforms for transformation matrices
uniform mat4 model;         // Model matrix
uniform mat4 view;          // View matrix
uniform mat4 projection;    // Projection matrix

void main()
{
    // Calculate the normal matrix
    mat4 normalMatrix = transpose(inverse(model));

    // Transform the normal from object to view space
    surface_normal = vec3(normalMatrix * normal);

    // Transform the vertex position to view space
    surface_position = vec3(view * model * position); // Combine view and model transformations

    eyeSpaceVertexPos = view * model * position;
    interp_eyePos = vec3(-eyeSpaceVertexPos);

    // Pass texture coordinates to the fragment shader
    surface_texcoord = texcoord;
    varyingtexcoord = texcoord;

    // Transform the vertex position by model, view, and projection matrices
    gl_Position = projection * view * model * position; // Combine all transformations
}