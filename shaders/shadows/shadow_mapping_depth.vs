#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
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
};
uniform Light lights[MAX_LIGHTS];

//lightCount
uniform int lightCount;

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
            //gl_Position = light.lightSpaceMatrix * model * vec4(aPos, 1.0);
        }
        else if (light.type == 2) // Directional light
        {
            gl_Position = light.lightSpaceMatrix * model * vec4(aPos, 1.0);   
        }
        else if (light.type == 3) // Spotlight
        {
            gl_Position = light.lightSpaceMatrix * model * vec4(aPos, 1.0);
        }
    }

    //gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}