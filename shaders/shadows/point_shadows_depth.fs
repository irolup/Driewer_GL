#version 330 core
in vec4 FragPos;

//uniform vec3 lightPos;
uniform float far_plane;

// Maximum number of lights
#define MAX_LIGHTS 5 // Adjust as needed

// Light struct
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

void main()
{

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (lights[i].type == 1)
        {
            float lightDistance = length(FragPos.xyz - lights[i].position);
            lightDistance = lightDistance / far_plane;
            gl_FragDepth = lightDistance;
        }
    }

    //float lightDistance = length(FragPos.xyz - lightPos);
    //
    //// map to [0;1] range by dividing by far_plane
    //lightDistance = lightDistance / far_plane;
    //
    //// write this as modified depth
    //gl_FragDepth = lightDistance;
}