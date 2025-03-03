#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec3 Tangent;
out mat3 TBN;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    TexCoords = aTexCoords;

    // Tangent
    Tangent = normalize(vec3(model * vec4(aTangent, 0.0)));
    // Normal
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    Normal = N;
    vec3 B = cross(N, Tangent);
    TBN = mat3(Tangent, B, Normal);

    gl_Position = projection * view * worldPos;

}