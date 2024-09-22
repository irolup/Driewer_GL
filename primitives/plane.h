#ifndef PLANE_H
#define PLANE_H

#include "../glad/glad.h"
#include <stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include "../shaders/shader.h"
#include <vector>

class Plane {
public:
    Plane();
    void draw(Shader& shader, glm::vec3 planePosition, glm::vec3 cameraPos);
    
private:
    unsigned int VAO, VBO;
    unsigned int texture1, texture2;

    std::vector<unsigned int> textures;
    void setupPlane();
};

#endif // PLANE_H