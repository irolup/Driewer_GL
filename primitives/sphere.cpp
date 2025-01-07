#include "sphere.h"

Sphere::Sphere() {
    //default stack and sector
    stacks = 18;
    sectors = 36;

    generateSphereVertices(sectors, stacks);
    setup();
    material.ambient = glm::vec3(1.0f, 0.0f, 0.0f);
    material.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
    material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    material.metallic = 0.0f;
    material.roughness = 0.5f;
    material.occlusion = 1.0f;
    material.brightness = 1.0f;
    material.fresnel_ior = glm::vec3(1.5f);
}

//ADD FUNCTION FOR HITBOX

void Sphere::generateSphereVertices(unsigned int sectors, unsigned int stacks) {
    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f;
    float s, t;

    float sectorStep = 2 * M_PI / sectors;
    float stackStep = M_PI / stacks;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stacks; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = cos(stackAngle);
        z = sin(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep;

            x = xy * cos(sectorAngle);
            y = xy * sin(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back((float)j / sectors);
            vertices.push_back((float)i / stacks);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

void Sphere::setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load texture
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image, create texture and generate mipmaps
    int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("texture/PBR_textures_2/diff.jpg", &width, &height, &nrChannels, 0);
    
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture diff" << std::endl;
    }
    stbi_image_free(data);

    //texture2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image, create texture and generate mipmaps
    data = stbi_load("texture/PBR_textures_2/norm.jpg", &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture norm" << std::endl;
    }
    stbi_image_free(data);

    textures_sphere.push_back(texture1);
    textures_sphere.push_back(texture2);
}

void Sphere::draw(Shader& shader, Camera& camera) {
    shader.Use();
    glm::mat4 projection = camera.GetProjectionMatrix(0.1f, 100.0f);
    shader.SetMatrix4("projection", projection);
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetFloat("pitch", camera.getPitch());
    shader.SetFloat("yaw", camera.getYaw());
    shader.SetMatrix4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, getPosition());
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



    for (unsigned int i = 0; i < textures_sphere.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures_sphere[i]);
    }

    shader.SetInteger("texture_diffuse", 0);
    shader.SetInteger("texture_normal", 1);
    shader.SetInteger("texture_metallic", 2);
    shader.SetInteger("texture_roughness", 3);
    shader.SetInteger("texture_occlusion", 4);


    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //unbind the texture
    for (unsigned int i = 0; i < textures_sphere.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}


void Sphere::drawWithShadow(Shader& shader, Camera& camera, unsigned int depthMap, bool pointLight) {
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

for (unsigned int i = 0; i < textures_sphere.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures_sphere[i]);
    }
    //bind depth map
    //glActiveTexture(GL_TEXTURE0 + textures_cube.size());
    //glBindTexture(GL_TEXTURE_2D, depthMap);



    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Unbind the textures
    for (unsigned int i = 0; i < textures_sphere.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

std::string Sphere::getInfo() const {
    return "Sphere";
}