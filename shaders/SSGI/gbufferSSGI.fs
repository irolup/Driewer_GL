#version 330 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoMetallic;
layout(location = 3) out vec4 gSpecularRoughness;
layout(location = 4) out vec4 gFresnelOcclusion;
layout(location = 5) out vec4 gAmbiantBrightness;
layout(location = 6) out vec4 gDepth;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in vec3 Tangent;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_metallic;
uniform sampler2D texture_roughness;
uniform sampler2D texture_occlusion;

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

//camera position
uniform vec3 viewPos;

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

vec3 perturb_normal(vec3 N, vec3 V, vec2 texcoord)
{
    vec3 map = texture(texture_normal, texcoord).xyz;
    map = map * 2.0 - 1.0; // Convert from [0,1] to [-1,1]

    mat3 TBN;
    if (length(Tangent) > 0.0) {
        // Use provided Tangent to construct the TBN matrix
        vec3 B = normalize(cross(N, Tangent)); // Binormal
        TBN = mat3(Tangent, B, N);
    } else {
        // Compute TBN using the cotangent frame method
        TBN = CotangentFrame(N, -V, texcoord);
    }
    return normalize(TBN * map);
}

void main(){
    gPosition = FragPos;

    vec3 normal = normalize(Normal);
    if (textureSize(texture_normal, 0).x > 0) {
        //normal = perturb_normal(normal, normalize(FragPos - viewPos), TexCoords);
        //only use texture normal
        normal = texture(texture_normal, TexCoords).xyz * 2.0 - 1.0;
    }
    gNormal = normal;

    if (textureSize(texture_diffuse, 0).x > 0){
        gAlbedoMetallic.rgb = texture(texture_diffuse, TexCoords).rgb * material.diffuse;
        gAlbedoMetallic.a = texture(texture_metallic, TexCoords).r * material.metallic;
    } else {
        gAlbedoMetallic.rgb = material.diffuse;
        gAlbedoMetallic.a = material.metallic;
    }

    if (textureSize(texture_roughness, 0).x > 0){
        gSpecularRoughness.a = texture(texture_roughness, TexCoords).r * material.roughness;
        gSpecularRoughness.rgb = material.specular;
    } else {
        gSpecularRoughness.a = material.roughness;
        gSpecularRoughness.rgb = material.specular;
    }

    if (textureSize(texture_occlusion, 0).x > 0){
        gFresnelOcclusion.rgb = material.fresnel_ior;
        gFresnelOcclusion.a = texture(texture_occlusion, TexCoords).r * material.occlusion;
    } else {
        gFresnelOcclusion.rgb = material.fresnel_ior;
        gFresnelOcclusion.a = material.occlusion;
    }

    gAmbiantBrightness = vec4(material.ambient, material.brightness);

    gDepth = vec4(gl_FragCoord.z);

}