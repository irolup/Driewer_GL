#ifndef SPHERE_H
#define SPHERE_H

#include "../glad/glad.h"
#include <stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include "../shaders/shader.h"
#include <vector>
#include <cmath>

class Sphere {
public:
    Sphere(unsigned int sectors = 36, unsigned int stacks = 18);
    void draw(Shader& shader, glm::vec3 spherePositions);

private:
    unsigned int VAO, VBO, EBO;
    unsigned int texture1, texture2;
    std::vector<unsigned int> textures;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void setupSphere(unsigned int sectors, unsigned int stacks);
    void generateSphereVertices(unsigned int sectors, unsigned int stacks);
};

#endif // SPHERE_H