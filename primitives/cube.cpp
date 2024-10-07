#include "cube.h"

// Vertex data for a cube with positions and texture coordinates


Cube::Cube() {
    setup();

    material.ambient = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    material.diffuse = glm::vec4(1.0f, 0.5f, 0.31f, 1.0f);
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

    // Add textures to the vector
    textures.push_back(texture1);
    textures.push_back(texture2);

    // Hitbox setup
    glGenVertexArrays(1, &hitboxVAO);
    glGenBuffers(1, &hitboxVBO);
    glGenBuffers(1, &hitboxEBO);

    glBindVertexArray(hitboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, hitboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hitbox_vertices), hitbox_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hitboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hitbox_indices), hitbox_indices, GL_STATIC_DRAW);

    // Position attribute for hitbox
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);




    updateHitbox();
}

void Cube::draw(Shader& shader, Camera& camera) {
    shader.Use();

    glm::mat4 projection = camera.GetProjectionMatrix();
    shader.SetMatrix4("projection", projection);
    
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetMatrix4("view", view);
    
    //light position same as camera position
    glm::vec3 lightPos = camera.Position;
    shader.SetVector3f("lightPos", lightPos);

    // Set the view position
    glm::vec3 viewPos = camera.Position;
    shader.SetVector3f("viewPos", viewPos);

    //lightcolor for testing lightColor
    shader.SetVector4f("lightColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    //material
    shader.SetVector4f("material.ambient", material.ambient);
    shader.SetVector4f("material.diffuse", material.diffuse);
    shader.SetVector4f("material.specular", material.specular);
    shader.SetVector4f("material.emission", material.emission);
    shader.SetFloat("material.shininess", material.shininess);

    // Set the texture units
    shader.SetInteger("texture_diffuse1", 0);
    shader.SetInteger("texture_normal1", 1);
    
    // Bind texture
    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }
    // Set the model transformation matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, getPosition());
    shader.SetMatrix4("model", model);

    // Draw the cube using indices
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Cube::drawHitbox(Shader& shader, Camera& camera) {
    position = getPosition();
    shader.Use();
    glm::mat4 projection = camera.GetProjectionMatrix();
    shader.SetMatrix4("projection", projection);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, getHitboxPosition());
    shader.SetMatrix4("model", model);

    // Draw hitbox using indices
    glBindVertexArray(hitboxVAO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0); // Draw the hitbox edges
}


//set pos
void Cube::setPosition(glm::vec3 pos) {
    position = pos;
    updateHitbox();
}

std::string Cube::getInfo() const {
    return "Cube";
}