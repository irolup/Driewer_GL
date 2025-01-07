#version 330 core

// Constants
const float PI = 3.14159265358979323846;
//Height scale for parallax mapping
const float heightScale = 0.1;

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
uniform float pitch;
uniform float yaw;
uniform float near_plane;
uniform float far_plane;


// Inputs from vertex shader
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 Tangent;
in mat3 TBN;
in vec4 FragPosLightSpace;

// Output to framebuffer
out vec4 FragColor;

// Textures
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_metallic;
uniform sampler2D texture_roughness;
uniform sampler2D texture_occlusion;
uniform sampler2D texture_disp;
uniform sampler2D shadowMap;
uniform samplerCube shadowMapCube;

// Function declarations
float trowbridge_reitz(vec3 N, vec3 H, float roughness);
float schlick_beckmann(float cosTheta, float roughness);
float smith(vec3 N, vec3 L, vec3 V, float roughness);
vec3 schlick_fresnel(float cosTheta, vec3 F0);
vec3 tone_mapping_reinhard(vec3 color);
vec3 tone_mapping_aces_filmic(vec3 color);
vec3 CalculateLightingPBR(Light light, vec3 N, vec3 V, vec3 fragPos, vec3 albedo, float metallic, float roughness, float ao);

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

//takes the fragPosLightSpace, normal and the light position
float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightPos)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

float ShadowCalculationSpot(vec4 fragPosLightSpace, vec3 lightPos, vec3 spotlightDir, float cutoffAngle, float outerCutoffAngle)
{
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Check if fragment is outside the spotlight's frustum
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0; // Not in shadow since it's outside the light's view frustum

    // Spotlight attenuation
    vec3 fragToLight = normalize(lightPos - FragPos);
    float theta = dot(fragToLight, normalize(-spotlightDir));
    float epsilon = cutoffAngle - outerCutoffAngle;
    float attenuation = clamp((theta - outerCutoffAngle) / epsilon, 0.0, 1.0);

    if (attenuation <= 0.0)
        return 0.0; // Fragment is outside the spotlight's influence

    // Get closest depth value from shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // Calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // PCF (Percentage Closer Filtering)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    // Scale shadow by spotlight attenuation
    shadow *= attenuation;

    return shadow;
}

float ShadowCalculationPoint(vec3 lightPos, float far_plane, vec3 gridSamplingDisk[20])
{
    // get vector between fragment position and light position
    vec3 fragToLight = FragPos - lightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - FragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowMapCube, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    return shadow;

}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 0.0), viewDir)));
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDir.xy * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(texture_disp, currentTexCoords).r;

    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(texture_disp, currentTexCoords).r;
        currentLayerDepth += layerDepth;  
    }

    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(texture_disp, prevTexCoords).r - currentLayerDepth + layerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}



mat3 CotangentFrame(in vec3 N, in vec3 p, in vec2 uv) {
  vec3 dp1 = dFdx(p);
  vec3 dp2 = dFdy(p);
  vec2 duv1 = dFdx(uv);
  vec2 duv2 = dFdy(uv);

  vec3 dp2perp = cross(dp2, N);
  vec3 dp1perp = cross(N, dp1);
  vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
  vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

  float invmax = inversesqrt(max(dot(T, T), dot(B, B)));
  return mat3(T * invmax, B * invmax, N);
}

vec3 getNormalFromMap(vec2 texcoord){
    vec3 tangentNormal = texture(texture_normal, texcoord).xyz * 2.0 - 1.0;

    vec3 T = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 B = normalize(cross(Normal, T));
    mat3 TBN = mat3(T, B, Normal);

    return normalize(TBN * tangentNormal);
}


// Perturb normal using normal map
vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord )
{
    // assume N, the interpolated vertex normal and
    // V, the view vector (vertex to eye)
    vec3 map = texture(texture_normal, texcoord ).xyz;
    map = map * 2.0 - 1.0;
    mat3 TBN = CotangentFrame(N, -V, texcoord);
    return normalize(TBN * map);
}

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
vec3 CalculateLightingPBR(Light light, vec3 N, vec3 V, vec3 fragPos, vec3 albedo, float metallic, float roughness, float ao, float shadow)
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
        vec3 lightDir = normalize(light.position - fragPos);
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

    //radiance *= (1.0 - shadow); // Apply shadow

    // Final reflected light
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    Lo *= (1.0 - shadow); // Apply shadow

    
    vec3 color = ( ambient + Lo ) * material.brightness;

    return color;
}

void main()
{
    // Normalize interpolated normal
    vec3 N = normalize(Normal);
    
    // View direction
    vec3 V = normalize(viewPos - FragPos); //or -FragPos

    vec2 newTexCoords = TexCoords;

    //discard if outside the texture
    //if(newTexCoords.x > 1.0 || newTexCoords.y > 1.0 || newTexCoords.x < 0.0 || newTexCoords.y < 0.0)
    //    discard;

    // Retrieve material properties from textures
    // Retrieve material properties from textures
    vec3 albedo = material.diffuse; // Default to material color
    
    // Check if the diffuse texture is available
    if (textureSize(texture_diffuse, 0).x > 0) {
        albedo = texture(texture_diffuse, newTexCoords).rgb * material.diffuse;
    }
    
    float metallic = material.metallic; // Default to material metallic
    
    // Check if the metallic texture is available
    if (textureSize(texture_metallic, 0).x > 0) {
        metallic = texture(texture_metallic, newTexCoords).r * material.metallic;
    }
    
    float roughness = material.roughness; // Default to material roughness
    
    // Check if the roughness texture is available
    if (textureSize(texture_roughness, 0).x > 0) {
        roughness = texture(texture_roughness, newTexCoords).r * material.roughness;
    }
    
    float ao = material.occlusion; // Default to material occlusion
    
    // Check if the occlusion texture is available
    if (textureSize(texture_occlusion, 0).x > 0) {
        ao = texture(texture_occlusion, newTexCoords).r * material.occlusion;
    }

    // Apply normal mapping if normal map is provided
    if (textureSize(texture_normal, 0).x > 0)
    {
        //if no tangent is provided use perturb_normal
        //if (Tangent == vec3(0.0))
            N = normalize(perturb_normal(N, V, newTexCoords));
        //else
        //    N = getNormalFromMap(newTexCoords);

        //N = normalize(perturb_normal(N, V, newTexCoords));
        //N = getNormalFromMap(newTexCoords);
    }



    // Accumulate lighting
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
            // Calculate shadow
            float shadow = ShadowCalculationPoint(light.position, far_plane, gridSamplingDisk);
            lighting += CalculateLightingPBR(light, N, V, FragPos, albedo, metallic, roughness, ao, 0.0);
        }
        else if (light.type == 2) // Directional light
        {
            // For directional lights, direction is used instead of position
            float shadow = ShadowCalculation(FragPosLightSpace, light.position);
            lighting += CalculateLightingPBR(light, N, V, FragPos, albedo, metallic, roughness, ao, shadow);
        }
        else if (light.type == 3) // Spotlight
        {
            vec3 L = normalize(light.position - FragPos);
            float theta = dot(L, normalize(-light.direction));
            float epsilon = light.cutOff - light.outerCutOff;
            float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

            if (intensity > 0.0)
            {
                float shadow = ShadowCalculationSpot(FragPosLightSpace, light.position, light.direction, light.cutOff, light.outerCutOff);
                vec3 spotlightRadiance = CalculateLightingPBR(light, N, V, FragPos, albedo, metallic, roughness, ao, shadow) * intensity;
                lighting += spotlightRadiance;
            }
        }
    }

    // Apply brightness and ambient occlusion
    //lighting *= material.brightness;
    //lighting = mix(lighting, lighting, material.occlusion);

    // Tone mapping (adjust exposure and gamma as needed)
    //vec3 color = tone_mapping_reinhard(lighting);
    vec3 color = tone_mapping_aces_filmic(lighting);

    color = pow(color, vec3(1.0 / 2.2)); // Apply gamma correction

    // Output final color
    //test the normal map texture
    
    FragColor = vec4((color), 1.0);
    //trie debug with only shadows
    
    //float depthValue = texture(shadowMap, TexCoords).r;
    //FragColor = vec4(vec3(depthValue), 1.0);

}