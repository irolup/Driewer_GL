#include "cube.h"

// Vertex data for a cube with positions and texture coordinates


Cube::Cube() {
    setup();

    //material.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
    //material.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
    //material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    //material.metallic = 0.0f;
    //material.roughness = 0.5f;
    //material.occlusion = 1.0f;
    //material.brightness = 1.0f;
    //material.fresnel_ior = glm::vec3(1.5f);

    //material like metal
    material.ambient = glm::vec3(0.5f, 0.5f, 0.5f);    // Metals typically don't have ambient color contribution
    material.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);    // Metals often reflect their environment, so their diffuse color is more neutral
    material.specular = glm::vec3(0.1f, 0.1f, 0.1f);   // High specular highlights for shiny surfaces
    material.metallic = 1.0f;                          // Full metallic value to simulate metal
    material.roughness = 0.3f;                         // Low roughness for a smooth, shiny surface
    material.occlusion = 1.0f;                       // Occlusion usually remains at full value
    material.brightness = 1.0f;                        // Keep brightness at standard level
    material.fresnel_ior = glm::vec3(2.5f);            // Higher IOR for metals, as they tend to reflect more light at glancing angles
}

void Cube::setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
//
    glBindVertexArray(VAO);
//
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
//
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
//
    // Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Tangents
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    
//
    glBindVertexArray(0);


    // Load diffuse texture
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
    unsigned char *data = stbi_load("texture/PBR_textures_2/diff.jpg", &width, &height, &nrChannels, 0);
    
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        textures_cube.push_back(texture_diffuse);
    } else {
        std::cout << "Failed to load texture diff" << std::endl;
    }
    
    stbi_image_free(data);

    //texture normal
    glGenTextures(1, &texture_normal);
    glBindTexture(GL_TEXTURE_2D, texture_normal);
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
        textures_cube.push_back(texture_normal);
    } else {
        std::cout << "Failed to load texture norm" << std::endl;
    }
    stbi_image_free(data);

    //texture metallic
    glGenTextures(1, &texture_metalllic);
    glBindTexture(GL_TEXTURE_2D, texture_metalllic);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image, create texture and generate mipmaps
    data = stbi_load("texture/PBR_textures_2/met.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        if (nrChannels == 1) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
        textures_cube.push_back(texture_metalllic);
    } else {
        std::cout << "Failed to load texture met" << std::endl;
    }
    stbi_image_free(data);

    //tex roughness
    glGenTextures(1, &texture_roughness);
    glBindTexture(GL_TEXTURE_2D, texture_roughness);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image, create texture and generate mipmaps
    data = stbi_load("texture/PBR_textures_2/rough.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        if (nrChannels == 1) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
        textures_cube.push_back(texture_roughness);
    } else {
        std::cout << "Failed to load texture rough" << std::endl;
    }
    stbi_image_free(data);

    

    glGenTextures(1, &texture_ao);
    glBindTexture(GL_TEXTURE_2D, texture_ao);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image, create texture and generate mipmaps
    data = stbi_load("texture/PBR_textures_2/ao.jpg", &width, &height, &nrChannels, 0);
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
        textures_cube.push_back(texture_ao);
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
        textures_cube.push_back(texture_disp);
    } else {
        std::cout << "Failed to load AO texture" << std::endl;
    }
    stbi_image_free(data);





    updateHitbox();
}

void Cube::draw(Shader& shader, Camera& camera) {
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

for (unsigned int i = 0; i < textures_cube.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures_cube[i]);
    }


    // Set the texture units
    
    
    // Bind texture
    
    // Draw the cube using indices
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Unbind the textures
    for (unsigned int i = 0; i < textures_cube.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Cube::drawWithShadow(Shader& shader, Camera& camera, unsigned int depthMap) {
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

for (unsigned int i = 0; i < textures_cube.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures_cube[i]);
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
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Unbind the textures
    for (unsigned int i = 0; i < textures_cube.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // Unbind the depth map
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    
}

void Cube::drawTest(Shader& shader, Camera& camera){
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
    //shader.SetFloat("pitch", camera.getPitch());
    //shader.SetFloat("yaw", camera.getYaw());

    //material
    //shader.SetVector3f("material.ambient", material.ambient);
    //shader.SetVector3f("material.diffuse", material.diffuse);
    //shader.SetVector3f("material.specular", material.specular);
    //shader.SetFloat("material.metallic", material.metallic);
    //shader.SetFloat("material.roughness", material.roughness);
    //shader.SetFloat("material.occlusion", material.occlusion);
    //shader.SetFloat("material.brightness", material.brightness);
    //shader.SetVector3f("material.fresnel_ior", material.fresnel_ior);

    //shader.SetInteger("texture_diffuse", 1);
    //shader.SetInteger("texture_normal", 1);
    //shader.SetInteger("texture_metallic", 2);
    //shader.SetInteger("texture_roughness", 3);
    //shader.SetInteger("texture_occlusion", 4);
    //shader.SetInteger("texture_disp", 5);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_diffuse);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Unbind the textures
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
}

//set pos
void Cube::setPosition(glm::vec3 pos) {
    position = pos;
    updateHitbox();
}

std::string Cube::getInfo() const {
    return "Cube";
}