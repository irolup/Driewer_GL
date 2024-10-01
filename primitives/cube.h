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
    void draw(Shader& shader, glm::vec3 position, Camera& camera) override;

private:
    unsigned int VAO, VBO, EBO;
    //texture
    unsigned int texture1, texture2;
    std::vector<unsigned int> textures;

    //hiotbox
    unsigned int hitboxVAO, hitboxVBO, hitboxEBO;

    void drawHitbox(Shader& shader, glm::vec3 position, Camera& camera);

    void setup() override;

    std::string getInfo() const override;
};

#endif // CUBE_H