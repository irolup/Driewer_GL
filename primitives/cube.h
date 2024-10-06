#ifndef CUBE_H
#define CUBE_H

#include "../glad/glad.h"
#include <stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include "../shaders/shader.h"
//vector for texture
#include <vector>
#include "primitives.h"

class Cube : public Primitives {
public:
    Cube();
    void draw(Shader& shader, Camera& camera) override;
    void setPosition(glm::vec3 pos);

private:
    unsigned int VAO, VBO, EBO;
    //texture
    unsigned int texture1, texture2;
    std::vector<unsigned int> textures;

    //hiotbox
    unsigned int hitboxVAO, hitboxVBO, hitboxEBO;

    void drawHitbox(Shader& shader, Camera& camera);

    void setup() override;

    std::string getInfo() const override;

    //position of the cube with reference
    //glm::vec3 position;

    //set position of the cube
    


        // Cube vertex data
    static constexpr float cube_vertices[] = {
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

    // Cube index data
    static constexpr unsigned int cube_indices[] = {
        0, 1, 2, 2, 3, 0,  // Back face
        4, 5, 6, 6, 7, 4,  // Front face
        8, 9, 10, 10, 11, 8,  // Left face
        12, 13, 14, 14, 15, 12,  // Right face
        16, 17, 18, 18, 19, 16,  // Bottom face
        20, 21, 22, 22, 23, 20   // Top face
    };

    // Hitbox vertices for wireframe rendering
    static constexpr float hitbox_vertices[] = {
        -0.5f, -0.5f, -0.5f, // back-bottom-left
         0.5f, -0.5f, -0.5f, // back-bottom-right
         0.5f,  0.5f, -0.5f, // back-top-right
        -0.5f,  0.5f, -0.5f, // back-top-left
        -0.5f, -0.5f,  0.5f, // front-bottom-left
         0.5f, -0.5f,  0.5f, // front-bottom-right
         0.5f,  0.5f,  0.5f, // front-top-right
        -0.5f,  0.5f,  0.5f  // front-top-left
    };

    // Hitbox indices for drawing lines (edges)
    static constexpr unsigned int hitbox_indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,  // Back face edges
        4, 5, 5, 6, 6, 7, 7, 4,  // Front face edges
        0, 4, 1, 5, 2, 6, 3, 7   // Connecting edges
    };
};

#endif // CUBE_H