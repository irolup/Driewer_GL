#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 100;

uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 TexCoords;
out vec3 Normal0;
out vec3 WorldPos0;

void main()
{
    mat4 BoneTransform = mat4(0.0); // Initialize BoneTransform to zero

    // Compute the BoneTransform matrix based on the weights and bone IDs
    BoneTransform += finalBonesMatrices[boneIds[0]] * weights[0];
    BoneTransform += finalBonesMatrices[boneIds[1]] * weights[1];
    BoneTransform += finalBonesMatrices[boneIds[2]] * weights[2];
    BoneTransform += finalBonesMatrices[boneIds[3]] * weights[3];

    vec4 localPosition = BoneTransform * vec4(pos, 1.0);
    
    mat4 viewModel = view * model;
    gl_Position = projection * viewModel * localPosition;

    TexCoords = tex;

    // Compute the normal in world space
    vec4 normalL = BoneTransform * vec4(norm, 0.0);
    Normal0 = (model * normalL).xyz; // Use model matrix to transform the normal
    WorldPos0 = (model * localPosition).xyz; // World position in world space
}