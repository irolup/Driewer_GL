#include "plane.h"


Plane::Plane() {
    setup();

    // Material setup for a brick wall look
    material.ambient = glm::vec3(0.1f, 0.1f, 0.1f);    // A muted, earthy tone to reflect the color of bricks
    material.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);     // Brick color, typically a reddish or earthy color
    material.specular = glm::vec3(0.1f, 0.1f, 0.1f);    // Low specular to reduce shininess
    material.metallic = 0.0f;                          // Non-metallic material
    material.roughness = 0.7f;                         // Higher roughness for a more textured, rough surface
    material.occlusion = 1.0f;                         // Full occlusion to maintain depth in the material
    material.brightness = 1.0f;                        // Standard brightness
    material.fresnel_ior = glm::vec3(1.0f);            // IOR for non-metallic surfaces, typically around 1.0

}

void Plane::setup() {

    // positions at 25 and 0 for y
    glm::vec3 pos1(-1.0f, 0.0f, -1.0f);
    glm::vec3 pos2(1.0f, 0.0f, -1.0f);
    glm::vec3 pos3(1.0f, 0.0f, 1.0f);
    glm::vec3 pos4(-1.0f, 0.0f, 1.0f);
    // texture coordinates
    glm::vec2 uv1(0.0f, 0.0f);
    glm::vec2 uv2(1.0f, 0.0f);
    glm::vec2 uv3(1.0f, 1.0f);
    glm::vec2 uv4(0.0f, 1.0f);
    // normal vector
    glm::vec3 nm(0.0f, 1.0f, 0.0f);
    // calculate tangent/bitangent vectors of both triangles
    glm::vec3 tangent1, bitangent1;
    glm::vec3 tangent2, bitangent2;
    // triangle 1
    // ----------
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x); 
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    // triangle 2
    // ----------
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;
    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    //cout normal, tangent
    std::cout << "Normal: " << nm.x << " " << nm.y << " " << nm.z << std::endl;
    std::cout << "Tangent1: " << tangent1.x << " " << tangent1.y << " " << tangent1.z << std::endl;
    std::cout << "Tangent2: " << tangent2.x << " " << tangent2.y << " " << tangent2.z << std::endl;
    

    float plane_vertices[] = {
        pos1.x, pos1.y, pos1.z,   nm.x, nm.y, nm.z,  uv1.x, uv1.y,      tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos2.x, pos2.y, pos2.z,   nm.x, nm.y, nm.z,  uv2.x, uv2.y,      tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos3.x, pos3.y, pos3.z,   nm.x, nm.y, nm.z,  uv3.x, uv3.y,      tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

        // Second Triangle
        pos1.x, pos1.y, pos1.z,   nm.x, nm.y, nm.z,  uv1.x, uv1.y,      tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos3.x, pos3.y, pos3.z,   nm.x, nm.y, nm.z,  uv3.x, uv3.y,      tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos4.x, pos4.y, pos4.z,   nm.x, nm.y, nm.z,  uv4.x, uv4.y,      tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // Bind and set vertex buffer data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

    


    // Load texture
    glGenTextures(1, &texture_diffuse);
    glBindTexture(GL_TEXTURE_2D, texture_diffuse);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image, create texture and generate mipmaps
    int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("texture/PBR_textures/diff.jpg", &width, &height, &nrChannels, 0);
    
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture diff" << std::endl;
    }
    stbi_image_free(data);

    //texture2
    glGenTextures(1, &texture_normal);
    glBindTexture(GL_TEXTURE_2D, texture_normal);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image, create texture and generate mipmaps
    data = stbi_load("texture/PBR_textures/norm.jpg", &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture norm" << std::endl;
    }
    stbi_image_free(data);

    //texture mettalic
    glGenTextures(1, &texture_metalllic);
    glBindTexture(GL_TEXTURE_2D, texture_metalllic);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image, create texture and generate mipmaps
    data = stbi_load("texture/PBR_textures/met.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        if (nrChannels == 1) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture rough" << std::endl;
    }
    stbi_image_free(data);

    //texture roughness
    glGenTextures(1, &texture_roughness);
    glBindTexture(GL_TEXTURE_2D, texture_roughness);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image, create texture and generate mipmaps
    data = stbi_load("texture/PBR_textures/rough.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        if (nrChannels == 1) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture rough" << std::endl;
    }
    stbi_image_free(data);
    
//
    glGenTextures(1, &texture_ao);
    glBindTexture(GL_TEXTURE_2D, texture_ao);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image, create texture and generate mipmaps
    data = stbi_load("texture/PBR_textures/ao.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        // Check the number of channels and set the texture format accordingly
        if (nrChannels == 1) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
    
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load AO texture" << std::endl;
    }
    stbi_image_free(data);

    //Displacement map
    glGenTextures(1, &texture_disp);
    glBindTexture(GL_TEXTURE_2D, texture_disp);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image, create texture and generate mipmaps
    data = stbi_load("texture/PBR_textures_2/disp.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        // Check the number of channels and set the texture format accordingly
        if (nrChannels == 1) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }

        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load AO texture" << std::endl;
    }
    stbi_image_free(data);


    //add textures to vector
    textures_plane.push_back(texture_diffuse);
    textures_plane.push_back(texture_normal);
    textures_plane.push_back(texture_metalllic);
    textures_plane.push_back(texture_roughness);
    textures_plane.push_back(texture_ao);
    textures_plane.push_back(texture_disp);

    updateHitbox();
}

void Plane::draw(Shader& shader, Camera& camera) {
    shader.Use();
    glm::mat4 projection = camera.GetProjectionMatrix();
    shader.SetMatrix4("projection", projection);
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetMatrix4("view", view);

    glm::vec3 viewPos = camera.Position;
    shader.SetVector3f("viewPos", viewPos);
    shader.SetFloat("pitch", camera.getPitch());
    shader.SetFloat("yaw", camera.getYaw());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, getPosition());
    //scale more big
    model = glm::scale(model, scale);
    shader.SetMatrix4("model", model);

        //Materials
    shader.SetVector3f("material.ambient", material.ambient);
    shader.SetVector3f("material.diffuse", material.diffuse);
    shader.SetVector3f("material.specular", material.specular);
    shader.SetFloat("material.metallic", material.metallic);
    shader.SetFloat("material.roughness", material.roughness);
    shader.SetFloat("material.occlusion", material.occlusion);
    shader.SetFloat("material.brightness", material.brightness);
    shader.SetVector3f("material.fresnel_ior", material.fresnel_ior);

// Bind texture
    for (unsigned int i = 0; i < textures_plane.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures_plane[i]);
    }

    shader.SetInteger("texture_diffuse", 0);
    shader.SetInteger("texture_normal", 1);
    shader.SetInteger("texture_metallic", 2);
    shader.SetInteger("texture_roughness", 3);
    shader.SetInteger("texture_occlusion", 4);
    shader.SetInteger("texture_disp", 5);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6); // 6 vertices for 2 triangles
    glBindVertexArray(0);

    // Unbind the textures
    for (unsigned int i = 0; i < textures_plane.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

std::string Plane::getInfo() const {
    return "Plane";
}

void Plane::drawWithShadow(Shader& shader, Camera& camera, unsigned int depthMap) {
    shader.Use();
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, getPosition());
    model = glm::scale(model, scale);

    shader.SetMatrix4("model", model);
    
    glm::mat4 projection = camera.GetProjectionMatrix();
    shader.SetMatrix4("projection", projection);
    
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetMatrix4("view", view);

    // Set the view position
    glm::vec3 viewPos = camera.Position;
    shader.SetVector3f("viewPos", viewPos);

    //pitch
    shader.SetFloat("pitch", camera.getPitch());
    shader.SetFloat("yaw", camera.getYaw());

    //material
    shader.SetVector3f("material.ambient", material.ambient);
    shader.SetVector3f("material.diffuse", material.diffuse);
    shader.SetVector3f("material.specular", material.specular);
    shader.SetFloat("material.metallic", material.metallic);
    shader.SetFloat("material.roughness", material.roughness);
    shader.SetFloat("material.occlusion", material.occlusion);
    shader.SetFloat("material.brightness", material.brightness);
    shader.SetVector3f("material.fresnel_ior", material.fresnel_ior);

    shader.SetInteger("texture_diffuse", 0);
    shader.SetInteger("texture_normal", 1);
    shader.SetInteger("texture_metallic", 2);
    shader.SetInteger("texture_roughness", 3);
    shader.SetInteger("texture_occlusion", 4);
    shader.SetInteger("texture_disp", 5);
    shader.SetInteger("shadowMap", 6);
    shader.SetInteger("shadowMapCube", 7);

    for (unsigned int i = 0; i < textures_plane.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures_plane[i]);
    }
    //bind depth map
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);

    // Set the texture units
    
    
    // Bind texture
    
    // Draw the cube using indices
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Unbind the textures
    for (unsigned int i = 0; i < textures_plane.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // Unbind the depth map
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Plane::drawTest(Shader& shader, Camera& camera) {}