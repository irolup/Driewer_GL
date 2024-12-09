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
    unsigned int VAO, VBO, EBO;
    unsigned int texture_diffuse, texture_normal, texture_metalllic, texture_roughness, texture_ao, texture_disp;

    std::vector<unsigned int> textures_plane;
    
    void setup() override;

    std::string getInfo() const override;

    static constexpr float plane_vertices[] = {
        // positions         // normals         // texture coords // tangents
        -0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  1.0f, 0.0f, // Bottom-left
         0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  1.0f, 0.0f, // Bottom-right
         0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  1.0f, 0.0f, // Top-right
        -0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,  1.0f, 0.0f  // Top-left
    };

    // Indices for the plane (two triangles)
    static constexpr unsigned int plane_indices[] = {
        0, 1, 2,  // First triangle
        2, 3, 0   // Second triangle
    };


};

#endif // PLANE_H