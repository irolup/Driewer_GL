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
#include "../lights/lights.h"

class Cube : public Primitives {
public:
    Cube();
    void draw(Shader& shader, Camera& camera) override;
    //draw with voxel shader
    void setPosition(glm::vec3 pos);
    void drawWithShadow(Shader& shader, Camera& camera, std::vector<Light::LightData*> lights, unsigned int depthMap);

private:
    unsigned int VAO, VBO, EBO;
    //texture
    unsigned int texture_diffuse, texture_normal, texture_metalllic, texture_roughness, texture_ao, texture_disp;
    std::vector<unsigned int> textures_cube;

    void setup() override;

    std::string getInfo() const override;

    //position of the cube with reference
    //glm::vec3 position;

    //set position of the cube
    


        // Cube vertex data
    static constexpr float cube_vertices[] = {
        // positions                 // normals           // texture coords // tangents
        // Back face
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-left
        //front 11.0
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f, 0.0f,  1.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-left
        //left f1.0
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-left
        -1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-right
        //right 11.0
         1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-right
         1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-left
        //bottom1.1.0
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-right
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-left
        //top f1.0
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
         1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f  // bottom-left
    };


    

    static constexpr unsigned int cube_indices[] = {
        // Back face
        0, 1, 2, // First triangle
        2, 3, 0, // Second triangle
        // Front face
        4, 5, 6, // First triangle
        6, 7, 4, // Second triangle
        // Left face
        8, 9, 10, // First triangle
        10, 11, 8, // Second triangle
        // Right face
        12, 13, 14, // First triangle
        14, 15, 12, // Second triangle
        // Bottom face
        16, 17, 18, // First triangle
        18, 19, 16, // Second triangle
        // Top face
        20, 21, 22, // First triangle
        22, 23, 20  // Second triangle
    };


};

#endif // CUBE_H