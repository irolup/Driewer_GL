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
in mat3 TBN;

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


void main(){
    gPosition = FragPos;

    vec3 normal = normalize(Normal);
    if (textureSize(texture_normal, 0).x > 0) {

        normal = texture(texture_normal, TexCoords).xyz * 2.0 - 1.0;
        normal = normalize(TBN * normal);
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