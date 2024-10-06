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

// Textures
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

// Function to calculate point and directional lighting
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

// Function to calculate ambient lighting
vec3 CalculateAmbientLight(Light light) {
    return light.color.rgb * light.intensity; // Ambient light color and intensity
}

// Function to calculate directional lighting
vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color.rgb * light.intensity;

    // Specular lighting
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * light.color.rgb * light.intensity;

    return diffuse + specular;
}

// Function to calculate spotlight lighting
vec3 CalculateSpotlight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos) {
    vec3 lightDir = normalize(light.direction);
    float theta = dot(lightDir, normalize(fragPos - light.position));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    return CalculateLighting(light, normal, viewDir, fragPos) * intensity;
}

void main() {
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 lighting = vec3(0.0);
    
    // Phase 1: Calculate ambient lighting
    for (int i = 0; i < lightCount; i++) {
        if (lights[i].type == 0) { // Ambient light
            lighting += CalculateAmbientLight(lights[i]);
        }
    }

    // Phase 2: Calculate point lights
    for (int i = 0; i < lightCount; i++) {
        if (lights[i].type == 1) { // Point light
            lighting += CalculateLighting(lights[i], normal, viewDir, FragPos);
        }
    }

    // Phase 3: Calculate directional lights
    for (int i = 0; i < lightCount; i++) {
        if (lights[i].type == 2) { // Directional light
            lighting += CalculateDirectionalLight(lights[i], normal, viewDir);
        }
    }

    // Phase 4: Calculate spotlights
    for (int i = 0; i < lightCount; i++) {
        if (lights[i].type == 3) { // Spotlight
            lighting += CalculateSpotlight(lights[i], normal, viewDir, FragPos);
        }
    }

    FragColor = vec4(lighting, 1.0);
}