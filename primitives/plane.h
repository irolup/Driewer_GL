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
    void draw(Shader& shader, Camera& camera) override;
    
private:
    unsigned int VAO, VBO;
    unsigned int texture_diffuse, texture_normal, texture_metalllic, texture_roughness, texture_ao, texture_disp;

    std::vector<unsigned int> textures_plane;
    
    void setup() override;

    std::string getInfo() const override;




};

#endif // PLANE_H