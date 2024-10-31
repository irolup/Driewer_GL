#ifndef SPHERE_H
#define SPHERE_H

#include "../glad/glad.h"
#include <stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include "../shaders/shader.h"
#include <vector>
#include <cmath>
#include "primitives.h"

class Sphere : public Primitives {
public:
    Sphere();
    void draw(Shader& shader, Camera& camera) override;

private:
    unsigned int VAO, VBO, EBO;
    unsigned int texture1, texture2;
    std::vector<unsigned int> textures_sphere;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    unsigned int stacks;
    unsigned int sectors;

    void setup() override;
    void generateSphereVertices(unsigned int sectors, unsigned int stacks);

    std::string getInfo() const override;
};

#endif // SPHERE_H