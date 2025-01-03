#version 330 core

const float PI = 3.14159265358979323846;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoMetallic;
uniform sampler2D gSpecularRoughness;
uniform sampler2D gFresnelOcclusion;
uniform sampler2D gAmbiantBrightness;
uniform sampler2D gDepth;

//uniform for indirect lighting
uniform float ao_slider;

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
const int MAX_LIGHTS = 32;

uniform Light lights[MAX_LIGHTS];
uniform vec3 viewPos;

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

float smith(vec3 N, vec3 L, vec3 V, float roughness)
{
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float ggx2 = schlick_beckmann(NdotV, roughness);
    float ggx1 = schlick_beckmann(NdotL, roughness);
    return ggx1 * ggx2;
}

// Schlick-Beckmann approximation for geometry function


vec3 schlick_fresnel(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 CalculateLightingPBR(Light light, vec3 N, vec3 V, vec3 fragPos, vec3 albedo, float metallic, float roughness, 
                        float ao, vec3 fresnel, vec3 specular_, float brightness){
    //ambient is ambient with albedo * ao

    //ambiant is from the material

    vec3 ambient = albedo * ao * texture(gAmbiantBrightness, TexCoords).rgb;

    //Fresnel at normal incidence from fresnel value
    vec3 F0 = fresnel;

    //mix of F0 and albedo and metallic
    F0 = mix(F0, albedo, metallic);

    //light direction
    vec3 L;
    if (light.type == 2) // Directional light
    {
        L = normalize(-light.direction);
    }
    else // Point light or Spotlight
    {
        L = normalize(light.position - fragPos);
    }

    //halfway vector
    vec3 H = normalize(V + L);

    //NDF
    float NDF = trowbridge_reitz(N, H, roughness);

    //geometry
    float G = smith(N, L, V, roughness);

    //Fresnel
    vec3 F = schlick_fresnel(max(dot(H, V), 0.0), F0);

    //Specular reflection
    vec3 numerator = NDF * G * F;
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float denominator = 4.0 * NdotV * NdotL + 0.0001;
    vec3 specular = numerator / denominator;
    specular = specular * specular_;

    //Diffuse reflection
    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - metallic;

    vec3 radiance;
    if (light.type == 2) // Directional light have pos and dir
    {
        // Ensure light direction is normalized
        vec3 lightDir = normalize( light.position - fragPos );
        float NdotL = max(dot(N, lightDir), 0.0);    // Angle between normal and light direction
    
        // Radiance depends on light color, intensity, and NdotL
        radiance = light.color.rgb * light.intensity * NdotL;
    }
    else // Point light or Spotlight
    {
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (distance * distance); // Simple attenuation
        radiance = light.color.rgb * attenuation * light.intensity *50.0;
    }

    //final reflectred light
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 color = (ambient + Lo) * brightness;

    return color;


}

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

// Utility function to generate a random value between 0.0 and 1.0
float Hash(vec2 p) {
    p = fract(p * vec2(0.1031, 0.1030)); // Slightly different multiplier for x and y
    p += dot(p, p + vec2(33.333, 33.331));
    return fract(p.x * p.y);
}

// Generate random direction in the hemisphere based on normal
vec3 RandomHemisphereDirection(vec3 normal, float randomX, float randomY) {
    float theta = acos(sqrt(randomY)); // Cosine-weighted sampling
    float phi = 2.0 * 3.14159265359 * randomX;

    vec3 tangent1 = normalize(cross(abs(normal.x) > 0.1 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0), normal));
    vec3 tangent2 = cross(normal, tangent1);

    vec3 randomDir = sin(theta) * cos(phi) * tangent1 + sin(theta) * sin(phi) * tangent2 + cos(theta) * normal;
    return normalize(randomDir);
}

// Check if sample is occluded by geometry
bool Occluded(vec3 origin, vec3 direction, float maxDistance) {
    float t = 0.0;
    // Check occlusion along the ray in larger steps (increase step size to reduce performance cost)
    while (t < maxDistance) {
        float h = texture(gDepth, origin.xy + direction.xy * t).r;
        if (h > origin.z) {
            return true; // Geometry occluded the sample
        }
        t += 0.5; // Increase step size for fewer checks
    }
    return false;
}

// Sample the SSGI kernel
vec3 SampleSSGIKernel(vec3 N, vec3 V, vec3 P, float ao)
{
    vec3 result = vec3(0.0);
    float totalWeight = 0.0;
    int sampleCount = 8;  // Reduced number of samples to improve performance

    // Perform fewer samples to reduce computational cost
    for (int i = 0; i < sampleCount; i++) {
    // Divide the sample space into strata
        float stratumX = float(i) / float(sampleCount);
        float stratumY = fract(float(i) * 0.618034); // Use golden ratio for better stratification
    
        // Add jitter within the strata
        float randomX = stratumX + Hash(vec2(float(i), 0.0)) / float(sampleCount);
        float randomY = stratumY + Hash(vec2(float(i), 1.0)) / float(sampleCount);
    
        // Generate random direction
        vec3 L = RandomHemisphereDirection(N, randomX, randomY);
    
        // Skip invalid rays
        if (dot(L, N) < 0.0 || dot(L, V) < 0.0) {
            continue;
        }
    
        // Add contribution with fa
        float weight = max(0.0, dot(L, N));
        if (weight > 0.0) {
            //float occlusion = 1.0;
            float occlusion = ao_slider;
            if (Occluded(P, L, 10.0)) {
                occlusion = 0.0;
            }
            result += weight * occlusion;
            totalWeight += weight;
        }
    }

    // Avoid division by zero
    if (totalWeight > 0.0) {
        result /= totalWeight; // Normalize the result
    }

    // Scale down the indirect lighting to prevent overexposure
    float indirectStrength = 0.1; // Adjust this value to scale down indirect light intensity
    result *= indirectStrength;

    return result;
}

void main(){

    //seprarate the gbuffer data from the textures
    vec3 gPos = texture(gPosition, TexCoords).rgb;

    vec3 gNorm = texture(gNormal, TexCoords).rgb;

    //get just albedo from rgb and metallic from alpha
    vec4 gAlbedo = texture(gAlbedoMetallic, TexCoords);
    vec3 albedo = gAlbedo.rgb;
    float metallic = gAlbedo.a;

    //get specular from rgb and roughness from alpha
    vec4 gSpecular = texture(gSpecularRoughness, TexCoords);
    vec3 specular_ = gSpecular.rgb;
    float roughness = gSpecular.a;

    //get fresnel from rgb and occlusion from alpha
    vec4 gFresnel = texture(gFresnelOcclusion, TexCoords);
    vec3 fresnel = gFresnel.rgb;
    float ao = gFresnel.a;

    //get brightness
    float brightness = texture(gAmbiantBrightness, TexCoords).a;

    //get depth
    float depth = texture(gDepth, TexCoords).r;


    //hardcoded ambient light
    vec3 ambient_mat_color = texture(gAmbiantBrightness, TexCoords).rgb;

    //view direction
    vec3 V = normalize(viewPos - gPos);

    //apply normal mapping with perturb_normal
    vec3 N = normalize(gNorm);

    vec3 lighting = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        Light light = lights[i];


        //ambient light
        if (light.type == 0)
        {
            //ambient is ambient color with ambient intensity with albedo
            vec3 ambient = ambient_mat_color * albedo * light.color.rgb * light.intensity;
            lighting += ambient;
        } else if (light.type ==1) //Point light
        {
            lighting += CalculateLightingPBR(light, N, V, gPos, albedo, metallic, roughness, ao, fresnel, specular_, brightness);
        } else if (light.type == 2) //Directional light
        {
            lighting += CalculateLightingPBR(light, N, V, gPos, albedo, metallic, roughness, ao, fresnel, specular_, brightness);
        } else if (light.type == 3) //Spotlight
        {
            vec3 L = normalize(light.position - gPos);
            float theta = dot(L, normalize(-light.direction));
            float epsilon = light.cutOff - light.outerCutOff;
            float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

            if (intensity > 0.0)
            {
                lighting += CalculateLightingPBR(light, N, V, gPos, albedo, metallic, roughness, ao, fresnel, specular_, brightness) * intensity;
            }
            
        }
    }

    //SSGI
    //vec3 indirectLighting = vec3(0.0);
    //indirectLighting = SampleSSGIKernel(N, V, gPos, ao);
    //// Appliquer une force d'atténuation sur l'éclairage indirect pour éviter l'exposition excessive
    //float indirectStrength = 0.3; // Ajuste cette valeur selon tes besoins
    //indirectLighting *= indirectStrength;
    //// Ajouter l'éclairage indirect à l'éclairage direct
    //lighting += indirectLighting;
    //// Clamp de l'éclairage final pour éviter les valeurs excessives
    //lighting = clamp(lighting, 0.0, 1.0); // Empêcher l'exposition excessive


    //tone mapping ace filmic
    vec3 color = tone_mapping_aces_filmic(lighting);

    //gamma correction
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}