#version 330 core

struct Light {
    int type; // 0: ambient light, 1: point light, 2: directional light, 3: spotlight
    vec3 position;
    vec3 direction;
    vec4 color;
    float intensity;
    float cutOff;
    float outerCutOff;
};

#define MAX_LIGHTS 10 // Adjust as needed

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec3 ViewPos;

out vec4 FragColor;

uniform Light lights[MAX_LIGHTS];
uniform int lightCount; // Total number of lights
uniform vec3 viewPos;   // Camera position

// Function to calculate lighting
vec3 CalculateLighting(Light light, vec3 normal, vec3 viewDir, vec3 fragPos) {
    vec3 result = vec3(0.0);
    
    // Diffuse lighting
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color.rgb * light.intensity;

    // Specular lighting
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * light.color.rgb * light.intensity;

    // Final color
    result += diffuse + specular;

    return result;
}

void main() {
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 lighting = vec3(0.0);
    
    // First process AMBIENT lights (assume type 0)
    for (int i = 0; i < lightCount; i++) {
        if (lights[i].type == 0) { // Ambient light
            lighting += lights[i].color.rgb * lights[i].intensity; // Use ambient light color and intensity
        }
    }

    // Then process POINT lights (type 1)
    for (int i = 0; i < lightCount; i++) {
        if (lights[i].type == 1) { // Point light
            lighting += CalculateLighting(lights[i], normal, viewDir, FragPos);
        }
    }

    // Next, process DIRECTIONAL lights (type 2)
    for (int i = 0; i < lightCount; i++) {
        if (lights[i].type == 2) { // Directional light
            vec3 lightDir = normalize(-lights[i].direction);
            float diff = max(dot(normal, lightDir), 0.0);
            vec3 diffuse = diff * lights[i].color.rgb * lights[i].intensity;

            // Specular lighting
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = spec * lights[i].color.rgb * lights[i].intensity;

            lighting += diffuse + specular;
        }
    }

    // Finally, process SPOTLIGHT lights (type 3)
    for (int i = 0; i < lightCount; i++) {
        if (lights[i].type == 3) { // Spotlight
            vec3 lightDir = normalize(lights[i].direction);
            float theta = dot(lightDir, normalize(FragPos - lights[i].position));
            float epsilon = lights[i].cutOff - lights[i].outerCutOff;
            float intensity = clamp((theta - lights[i].outerCutOff) / epsilon, 0.0, 1.0);

            vec3 diffuse = CalculateLighting(lights[i], normal, viewDir, FragPos) * intensity;
            lighting += diffuse;
        }
    }

    FragColor = vec4(lighting, 1.0);
}