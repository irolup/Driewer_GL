#ifndef PLANE_H
#define PLANE_H

#include "../glad/glad.h"
#include <stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include "../shaders/shader.h"
#include <vector>
#include "primitives.h"

class Plane : public Primitives {
public:
    Plane();
    void draw(Shader& shader, glm::vec3 position, glm::vec3 cameraPos) override;
    
private:
    unsigned int VAO, VBO;
    unsigned int texture1, texture2;

    std::vector<unsigned int> textures;
    
    void setup() override;

    std::string getInfo() const override;
};

#endif // PLANE_H