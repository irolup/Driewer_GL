#ifndef CUBE_H
#define CUBE_H

#include "../glad/glad.h"
#include <stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include "../shaders/shader.h"
//vector for texture
#include <vector>

class Cube {
public:
    Cube();
    void draw(Shader& shader, glm::vec3 cubePositions, glm::vec3 cameraPos);

    //Material properties
        struct Material {
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        glm::vec4 emission;
        float shininess;
    } material;
    
private:
    unsigned int VAO, VBO;
    //texture
    unsigned int texture1, texture2;
    std::vector<unsigned int> textures;

    void setupCube();
};

#endif // CUBE_H