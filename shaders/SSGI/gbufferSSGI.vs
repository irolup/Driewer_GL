#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    vec3 FragPos = worldPos.xyz;
    vec2 TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 Normal = normalize(normalMatrix * vec3(aNormal, 0.0));
    
    gl_Position = projection * view * worldPos;

}