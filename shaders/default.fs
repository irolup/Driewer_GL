#version 330 core

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;

// Constants
const float PI = 3.14159265358979323846;
//Height scale for parallax mapping
const float heightScale = 0.1;

// Maximum number of lights
#define MAX_LIGHTS 5 // Adjust as needed

uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_metallic;
uniform sampler2D texture_roughness;
uniform sampler2D texture_occlusion;

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

out vec4 FragColor;


// Normal Distribution Function (NDF) - Trowbridge-Reitz GGX
float trowbridge_reitz(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float NdotH = max(dot(N, H), 0.0);
    float denom = (NdotH * NdotH * (a - 1.0) + 1.0);
    return (a * a) / (PI * denom * denom + 0.0001); // Avoid division by zero
}

float schlick_beckmann(float NdotX, float roughness)
{
    float k = (roughness + 1.0);
    k = (k * k) / 8.0;
    return NdotX / (NdotX * (1.0 - k) + k + 0.0001); // Avoid division by zero
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

// ACES Filmic tone mapping
vec3 tone_mapping_aces_filmic(vec3 color)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}


// Calculate PBR lighting using Cook-Torrance BRDF
vec3 CalculateLightingPBR(Light light, vec3 N, vec3 V, vec3 fragPos, vec3 albedo, float metallic, float roughness, float ao)
{


    vec3 ambient = material.ambient * albedo * ao; 

    // Fresnel reflectance at normal incidence
    vec3 F0 = material.fresnel_ior;

    F0 = mix(F0, albedo, metallic);
    //F0 = mix(vec3(0.04), albedo, metallic);

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

    specular = specular * material.specular;

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
        //light is too dark put higher intensity
        radiance = light.color.rgb * attenuation * light.intensity;
    }

    // Final reflected light
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 color = ( ambient + Lo ) * material.brightness;

    return color;
}

void main()
{
    //point light at 0,0 3
    vec3 lightPos = vec3(0.0, 3.0, 3.0);

    vec3 N = texture(texture_normal, TexCoords).rgb;
    N = normalize(N * 2.0 - 1.0);
    N = normalize(TBN * N);
    
    vec3 V = normalize(viewPos - FragPos);

    vec3 albedo = texture(texture_diffuse, TexCoords).rgb * material.diffuse;
    float metallic = texture(texture_metallic, TexCoords).r * material.metallic;
    float roughness = texture(texture_roughness, TexCoords).r * material.roughness;
    float ao = texture(texture_occlusion, TexCoords).r * material.occlusion;

    vec3 lighting = vec3(0.0);

    for (int i = 0; i < lightCount; i++)
    {
        Light light = lights[i];

        if (light.type == 0) // Ambient light
        {
            vec3 ambient = material.ambient * albedo * light.color.rgb * light.intensity;
            lighting += ambient;
        }
        else if (light.type == 1) // Point light
        {
            lighting += CalculateLightingPBR(light, N, V, FragPos, albedo, metallic, roughness, ao);
        }
    }



    
    //vec3 color = texture(texture_diffuse, TexCoords).rgb;
//
    //vec3 ambient = 0.1 * color;
    ////light direction for point light
    //vec3 lightDir = normalize(lightPos - FragPos);
    //float diff = max(dot(normal, lightDir), 0.0);
    //vec3 diffuse = diff * color;
    ////specular use tbn
    //vec3 viewDir = normalize(-FragPos);
    //vec3 reflectDir = reflect(-lightDir, normal);
    //vec3 halfwayDir = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
//
    //vec3 specular = vec3(0.2) * spec;
    

    vec3 finalColor = lighting;

    //FragColor = vec4(ambient + diffuse + specular, 1.0);
    FragColor = vec4(finalColor, 1.0);
}