#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos; // Fragment position in world space
in vec3 Normal;  // Normal vector in world space
in vec3 Tangent; // Tangent vector in world space

// Texture samplers
uniform sampler2D texture1; // Diffuse texture
uniform sampler2D texture2; // Normal map

// Material struct
struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    float shininess;
};

// Uniform for material properties
uniform Material material;

// Light properties
uniform vec3 lightPos; // Position of the light source
uniform vec3 viewPos;  // Position of the camera/viewer
uniform vec4 lightColor; // Color of the light

void main()
{
    // Sample the diffuse texture
    vec4 textureColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);

    // Normal mapping
    vec3 normalMap = texture(texture2, TexCoord).xyz * 2.0 - 1.0; // Read normal from normal map
    normalMap = normalize(normalMap); // Normalize the normal vector

    // Calculate TBN matrix
    vec3 T = normalize(Tangent);
    vec3 N = normalize(Normal);
    vec3 B = normalize(cross(N, T)); // Calculate bitangent
    mat3 TBN = mat3(T, B, N); // Tangent, Bitangent, Normal matrix

    // Transform normal from tangent space to world space
    vec3 normal = normalize(TBN * normalMap);

    // Ambient
    vec4 ambient = material.ambient * lightColor;

    // Diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec4 diffuse = material.diffuse * diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec4 specular = material.specular * spec * lightColor;

    // Combine results
    FragColor = textureColor * (ambient + diffuse + specular);
}