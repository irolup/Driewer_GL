#version 330 core

const float PI = 3.1415926535897932384626433832795;

struct Light {
    int type; // 0: ambient light, 1: point light, 2: directional light, 3: spotlight
    vec3 position;
    vec3 direction;
    vec4 color;
    float intensity;
    float cutOff;
    float outerCutOff;
};

#define MAX_LIGHTS 5 // Adjust as needed

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

// Uniform for material properties
uniform Material material;


in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 Tangent;

out vec4 FragColor;

uniform Light lights[MAX_LIGHTS];
uniform int lightCount; // Total number of lights
uniform vec3 viewPos;   // Camera position

// Textures
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_metallic;
uniform sampler2D texture_roughness;
uniform sampler2D texture_occlusion;

// fonction de distribution des microfacettes (Trowbridge-Reitz)
float trowbridge_reitz(vec3 n, vec3 h, float roughness)
{
  float a = roughness * roughness;
  float a2 = a * a;
  float ndh = max(dot(n, h), 0.0);
  float ndh2 = ndh * ndh;
  float numerator = a2;
  float denominator = (ndh2 * (a2 - 1.0) + 1.0);
  denominator = denominator * denominator * PI;
  return numerator / denominator;
}

// fonction géométrique pour calculer l'impact de l'occlusion et de l'ombrage des microfacettes (Schlick-Beckmann)
float schlick_beckmann(float costheta, float roughness)
{
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;
  float numerator = costheta;
  float denominator = costheta * (1.0 - k) + k;
  return numerator / denominator;
}

// fonction géométrique avec occlusion et ombrage combinés (méthode de Smith)
float smith(vec3 n, vec3 l, vec3 v, float roughness)
{
  float ndl = max(dot(n, l), 0.0);
  float ndv = max(dot(n, v), 0.0);
  float shadow = schlick_beckmann(ndl, roughness);
  float occlusion = schlick_beckmann(ndv, roughness);
  return shadow * occlusion;
}

// fonction qui calcul l'effet de Fresnel
vec3 schlick_fresnel(float costheta, vec3 f0)
{
  return f0 + (1.0 - f0) * pow(1.0 - costheta, 5.0);
}

// mappage tonal de la couleur HDR vers LDR (Reinhard tone mapping)
vec3 tone_mapping_reinhard(vec3 x)
{
  return x / (x + vec3(1.0));
}

// mappage tonal de la couleur HDR vers LDR (approximation de la courbe du ACES filmic tone mapping)
vec3 tone_mapping_aces_filmic(vec3 x)
{
  float a = 2.51f;
  float b = 0.03f;
  float c = 2.43f;
  float d = 0.59f;
  float e = 0.14f;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}


// Function to calculate point and directional lighting
vec3 CalculateLighting(Light light, vec3 normal, vec3 viewDir, vec3 fragPos) {
    vec3 result = vec3(0.0);

    vec3 ambient = light.color.rgb * vec3(texture(texture_diffuse, TexCoords));

    // Diffuse lighting
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color.rgb * light.intensity * vec3(texture(texture_diffuse, TexCoords));

    // Specular lighting
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * light.color.rgb * light.intensity;

    // Final color
    result += diffuse + specular + ambient;

    return result;
}

// Function to calculate ambient lighting
vec3 CalculateAmbientLight(Light light) {
    return light.color.rgb * light.intensity; // Ambient light color and intensity
}

// Function to calculate directional lighting
vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    //ambient
    vec3 ambient = light.color.rgb * vec3(texture(texture_diffuse, TexCoords));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color.rgb * light.intensity * vec3(texture(texture_diffuse, TexCoords));

    // Specular lighting
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * light.color.rgb * light.intensity; //need to add specular texture

    return (ambient + diffuse + specular);
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
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lighting = vec3(0.0);

    
    vec3 texture_sample_diffuse = texture(texture_diffuse, TexCoords).rgb;
    float texture_sample_metallic = texture(texture_metallic, TexCoords).r;
    float texture_sample_roughness = texture(texture_roughness, TexCoords).r;
    float texture_sample_occlusion = texture(texture_occlusion, TexCoords).r;

    float metallic = material.metallic * texture_sample_metallic;
    float roughness = material.roughness * texture_sample_roughness;
    float occlusion = material.occlusion * texture_sample_occlusion;

    vec3 albedo = material.diffuse * vec3(texture(texture_diffuse, TexCoords));

    vec3 ambient = material.ambient * albedo * occlusion;


    //Normal mapping
    vec3 normalMap = texture(texture_normal, TexCoords).xyz * 2.0 - 1.0;
    normalMap = normalize(normalMap);

    //Calculate TBN matrix
    vec3 T = normalize(Tangent);
    vec3 N = normalize(Normal);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    normal = normalize(TBN * normalMap);
    
    
    // Phase 1: Calculate ambient lighting
    for (int i = 0; i < lightCount; i++) {
        if (lights[i].type == 0) { // Ambient light
            lighting += CalculateAmbientLight(lights[i]);
        } else if
        (lights[i].type == 1) { // Point light
            lighting += CalculateLighting(lights[i], normal, viewDir, FragPos);
        } else if
        (lights[i].type == 2) { // Directional light
            lighting += CalculateDirectionalLight(lights[i], normal, viewDir);
        } else if
        (lights[i].type == 3) { // Spotlight
            lighting += CalculateSpotlight(lights[i], normal, viewDir, FragPos);
        }
        
    }

    FragColor = vec4(lighting, 1.0);
}