#include "plane.h"

// Vertex data for a plane
float plane_vertices[] = {
    // positions          // texture coords
    -0.5f, 0.0f, -0.5f,  0.0f, 0.0f,
     0.5f, 0.0f, -0.5f,  1.0f, 0.0f,
     0.5f, 0.0f,  0.5f,  1.0f, 1.0f,
     0.5f, 0.0f,  0.5f,  1.0f, 1.0f,
    -0.5f, 0.0f,  0.5f,  0.0f, 1.0f,
    -0.5f, 0.0f, -0.5f,  0.0f, 0.0f
};

Plane::Plane() {
    setup();
}

void Plane::setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);

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
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("texture/rock.jpg", &width, &height, &nrChannels, 0);
    
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
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
    data = stbi_load("texture/rock_norm.jpg", &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    //add textures to vector
    textures.push_back(texture1);
    textures.push_back(texture2);

    updateHitbox();
}

void Plane::draw(Shader& shader, glm::vec3 position, Camera& camera) {
    shader.Use();
    glm::mat4 projection = camera.GetProjectionMatrix(0.1f, 100.0f);
    shader.SetMatrix4("projection", projection);
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetMatrix4("view", view);
    shader.SetInteger("texture1", 0);
    shader.SetInteger("texture2", 1);

    // Bind texture
    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    //scale more big
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    shader.SetMatrix4("model", model);
    //Materials
    shader.SetVector4f("material.ambient", material.ambient);
    shader.SetVector4f("material.diffuse", material.diffuse);
    shader.SetVector4f("material.specular", material.specular);
    shader.SetVector4f("material.emission", material.emission);
    shader.SetFloat("material.shininess", material.shininess);

    //light pos
    shader.SetVector3f("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
    shader.SetVector3f("viewPos", camera.Position);
    shader.SetVector4f("lightColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Plane::drawHitbox(Shader& shader, glm::vec3 position, Camera& camera) {
    shader.Use();
    glm::mat4 projection = camera.GetProjectionMatrix(0.1f, 100.0f);
    shader.SetMatrix4("projection", projection);
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetMatrix4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    shader.SetMatrix4("model", model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

std::string Plane::getInfo() const {
    return "Plane";
}