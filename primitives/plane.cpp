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
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO); // Create Element Buffer Object

    glBindVertexArray(VAO);

    // Bind and set vertex buffer data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);

    // Bind and set element buffer data (indices)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_indices), plane_indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Tangent attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);


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
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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