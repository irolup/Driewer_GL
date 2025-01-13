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
out vec4 FragPosLightSpace;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

#define MAX_LIGHTS 5 // Adjust as needed

struct Light {
    int type; // 0: ambient light, 1: point light, 2: directional light, 3: spotlight
    vec3 position;
    vec3 direction;
    vec4 color;
    float intensity;
    float cutOff;
    float outerCutOff;
    mat4 lightSpaceMatrix;
    float far_plane;
};
uniform Light lights[MAX_LIGHTS];

uniform int lightCount; //lightCount


//uniform lightSpaceMatrix

void main()
{
    for (int i = 0; i < lightCount; i++)
    {
        Light light = lights[i];

        if (light.type == 0) // Ambient light
        {
            //do nothing
        }
        else if (light.type == 1) // Point light
        {
            //do nothing atm
            //FragPosLightSpace = light.lightSpaceMatrix * model * vec4(aPos, 1.0);
        }
        else if (light.type == 2) // Directional light
        {
            FragPosLightSpace = light.lightSpaceMatrix * model * vec4(aPos, 1.0);   
        }
        else if (light.type == 3) // Spotlight
        {
            FragPosLightSpace = light.lightSpaceMatrix * model * vec4(aPos, 1.0);
        }
    }

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