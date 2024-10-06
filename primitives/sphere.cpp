#include "sphere.h"

Sphere::Sphere() {
    //default stack and sector
    stacks = 18;
    sectors = 36;

    generateSphereVertices(sectors, stacks);
    setup();
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

    // Load texture 1
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("texture/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // Load texture 2
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

    textures.push_back(texture1);
    textures.push_back(texture2);
}

void Sphere::draw(Shader& shader, Camera& camera) {
    shader.Use();
    glm::mat4 projection = camera.GetProjectionMatrix(0.1f, 100.0f);
    shader.SetMatrix4("projection", projection);
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetMatrix4("view", view);
    shader.SetInteger("texture1", 0);
    shader.SetInteger("texture2", 1);

    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, getPosition());
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
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
}

//draw hitbox TO DO AND AJUST
void Sphere::drawHitbox(Shader& shader, Camera& camera) {
    shader.Use();
    glm::mat4 projection = camera.GetProjectionMatrix(0.1f, 100.0f);
    shader.SetMatrix4("projection", projection);
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetMatrix4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);
    shader.SetMatrix4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
}

std::string Sphere::getInfo() const {
    return "Sphere";
}