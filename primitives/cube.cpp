#include "cube.h"

// Vertex data for a cube with positions and texture coordinates
float cube_vertices[] = {
    // positions          // texture coords
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
    // Left face
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    // Right face
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-left
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left
};

// New index data (remains the same)
unsigned int cube_indices[] = {
    0, 1, 2, 2, 3, 0,  // Back face
    4, 5, 6, 6, 7, 4,  // Front face
    8, 9, 10, 10, 11, 8,  // Left face
    12, 13, 14, 14, 15, 12,  // Right face
    16, 17, 18, 18, 19, 16,  // Bottom face
    20, 21, 22, 22, 23, 20   // Top face
};

//hitbox vertices
float hitbox_vertices[] = {
    // positions
    -0.5f, -0.5f, -0.5f, // bottom-left
     0.5f, -0.5f, -0.5f, // bottom-right
     0.5f,  0.5f, -0.5f, // top-right
    -0.5f,  0.5f, -0.5f, // top-left
    // Front face
    -0.5f, -0.5f,  0.5f, // bottom-left
     0.5f, -0.5f,  0.5f, // bottom-right
     0.5f,  0.5f,  0.5f, // top-right
    -0.5f,  0.5f,  0.5f, // top-left
};

//hitbox indices
unsigned int hitbox_indices[] = {
    0, 1, 2, 2, 3, 0,  // Back face
    4, 5, 6, 6, 7, 4,  // Front face
};

Cube::Cube() {
    setup();

    material.ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    material.diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    material.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    material.emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    material.shininess = 32.0f; // Example shininess value
}

void Cube::setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO); // Element buffer object for indices

    glBindVertexArray(VAO);

    // Vertex buffer object (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // Element buffer object (EBO)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int texture1, texture2;
    // Texture 1 setup
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("texture/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // Texture 2 setup
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("texture/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // Add textures to the vector
    textures.push_back(texture1);
    textures.push_back(texture2);

    // Hitbox setup
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hitbox_vertices), hitbox_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hitbox_indices), hitbox_indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);




    updateHitbox();
}

// Draw the cube using indices
void Cube::draw(Shader& shader, glm::vec3 position, Camera& camera) {
    shader.Use();
    glm::mat4 projection = camera.GetProjectionMatrix(0.1f, 100.0f);
    shader.SetMatrix4("projection", projection);
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetMatrix4("view", view);
    shader.SetInteger("texture1", 0);
    shader.SetInteger("texture2", 1);

    // Materials
    shader.SetVector4f("material.ambient", material.ambient);
    shader.SetVector4f("material.diffuse", material.diffuse);
    shader.SetVector4f("material.specular", material.specular);
    shader.SetVector4f("material.emission", material.emission);
    shader.SetFloat("material.shininess", material.shininess);

    // Light and view positions
    shader.SetVector3f("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
    shader.SetVector3f("viewPos", camera.Position);
    shader.SetVector4f("lightColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    // Bind textures
    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }

    // Set the model transformation matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    shader.SetMatrix4("model", model);

    // Draw cube using indices
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Cube::drawHitbox(Shader& shader, glm::vec3 position, Camera& camera) {
    shader.Use();
    glm::mat4 projection = camera.GetProjectionMatrix(0.1f, 100.0f);
    shader.SetMatrix4("projection", projection);
    glm::mat4 view = camera.GetViewMatrix();
    // Set the model transformation matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    shader.SetMatrix4("model", model);

    // Draw hitbox using indices
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_LINE_LOOP, 8, GL_UNSIGNED_INT, 0);
}

std::string Cube::getInfo() const {
    return "Cube";
}