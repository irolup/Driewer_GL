#version 330 core

// Constants
const float PI = 3.14159265358979323846;

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
};

// Material struct
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float metallic;
    float roughness;
    float occlusion;
    float brightness;
    vec3 fresnel_ior;
};

// Uniforms
uniform Material material;
uniform Light lights[MAX_LIGHTS];
uniform int lightCount; // Total number of lights
uniform vec3 viewPos;   // Camera position

// Inputs from vertex shader
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 Tangent;

// Output to framebuffer
out vec4 FragColor;

// Function declarations
float trowbridge_reitz(vec3 N, vec3 H, float roughness);
float schlick_beckmann(float cosTheta, float roughness);
float smith(vec3 N, vec3 L, vec3 V, float roughness);
vec3 schlick_fresnel(float cosTheta, vec3 F0);
vec3 tone_mapping_reinhard(vec3 color);
vec3 CalculateLightingPBR(Light light, vec3 N, vec3 V, vec3 fragPos);

// Normal Distribution Function (NDF) - Trowbridge-Reitz GGX
float trowbridge_reitz(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float NdotH = max(dot(N, H), 0.0);
    float denom = (NdotH * NdotH * (a - 1.0) + 1.0);
    return (a * a) / (PI * denom * denom + 0.0001); // Avoid division by zero
}

// Geometry Function (Smith's method)
float smith(vec3 N, vec3 L, vec3 V, float roughness)
{
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float ggx2 = schlick_beckmann(NdotV, roughness);
    float ggx1 = schlick_beckmann(NdotL, roughness);
    return ggx1 * ggx2;
}

// Schlick-Beckmann approximation for geometry function
float schlick_beckmann(float NdotX, float roughness)
{
    float k = (roughness + 1.0);
    k = (k * k) / 8.0;
    return NdotX / (NdotX * (1.0 - k) + k + 0.0001); // Avoid division by zero
}

// Fresnel Equation (Schlick's approximation)
vec3 schlick_fresnel(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Reinhard tone mapping
vec3 tone_mapping_reinhard(vec3 color)
{
    return color / (color + vec3(1.0));
}

// Calculate PBR lighting using Cook-Torrance BRDF
vec3 CalculateLightingPBR(Light light, vec3 N, vec3 V, vec3 fragPos)
{
    vec3 albedo = material.diffuse;
    float metallic = material.metallic;
    float roughness = material.roughness;
    float ao = material.occlusion;

    vec3 ambient = material.ambient * albedo * ao;

    // Fresnel reflectance at normal incidence
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // Light direction
    vec3 L;
    if (light.type == 2) // Directional light
    {
        L = normalize(-light.direction);
    }
    else // Point light or Spotlight
    {
        L = normalize(light.position - fragPos);
    }

    // Half vector
    vec3 H = normalize(V + L);

    // NDF
    float NDF = trowbridge_reitz(N, H, roughness);

    // Geometry
    float G = smith(N, L, V, roughness);

    // Fresnel
    vec3 F = schlick_fresnel(max(dot(H, V), 0.0), F0);

    // Specular reflection
    vec3 numerator = NDF * G * F;
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float denominator = 4.0 * NdotV * NdotL + 0.0001; // Avoid division by zero
    vec3 specular = numerator / denominator;

    // Diffuse reflection
    vec3 kD = vec3(1.0) - F; // Energy conservation
    kD *= 1.0 - metallic;

    // Light radiance
    vec3 radiance;
    if (light.type == 2) // Directional light
    {
        radiance = light.color.rgb * light.intensity;
    }
    else // Point light or Spotlight
    {
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (distance * distance); // Simple attenuation
        radiance = light.color.rgb * attenuation * light.intensity;
    }

    // Final reflected light
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 color = (ambient + Lo) * material.brightness;

    return color;
}

void main()
{
    // Normalize interpolated normal
    vec3 N = normalize(Normal);

    // View direction
    vec3 V = normalize(viewPos - FragPos);

    // Accumulate lighting
    vec3 lighting = vec3(0.0);

    for (int i = 0; i < lightCount; i++)
    {
        Light light = lights[i];

        if (light.type == 0) // Ambient light
        {
            vec3 ambient = material.ambient * material.diffuse * light.color.rgb * light.intensity;
            lighting += ambient;
        }
        else if (light.type == 1) // Point light
        {
            lighting += CalculateLightingPBR(light, N, V, FragPos);
        }
        else if (light.type == 2) // Directional light
        {
            lighting += CalculateLightingPBR(light, N, V, FragPos);
        }
        else if (light.type == 3) // Spotlight
        {
            vec3 L = normalize(light.position - FragPos);
            float theta = dot(L, normalize(-light.direction));
            float epsilon = light.cutOff - light.outerCutOff;
            float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

            if (intensity > 0.0)
            {
                vec3 spotlightRadiance = CalculateLightingPBR(light, N, V, FragPos) * intensity;
                lighting += spotlightRadiance;
            }
        }
    }

    // Tone mapping (adjust exposure and gamma as needed)
    vec3 color = tone_mapping_reinhard(lighting);
    color = pow(color, vec3(1.0 / 2.2)); // Apply gamma correction

    // Output final color
    FragColor = vec4(color, 1.0);
}