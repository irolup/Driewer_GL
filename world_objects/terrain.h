#ifndef TERRAIN_H
#define TERRAIN_H

#include "../glad/glad.h"
#include <stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include "../shaders/shader.h"
#include <vector>
#include "../primitives/primitives.h"

class Terrain : public Primitives {
public:
    Terrain();
    void draw(Shader& shader, Camera& camera) override;

    std::vector<glm::vec3> vertices;

    //function to return the indices positions forcollision detection
    std::vector<glm::vec3> getVertices();
    
private:
    unsigned int VAO, VBO, EBO;
    unsigned int texture_diffuse, texture_normal, texture_metalllic, texture_roughness, texture_ao, texture_disp;
    unsigned int heightmap;

    std::vector<unsigned int> textures;

    void setup();
    void loadTextures();
    void generateTerrain();

    std::string getInfo() const override;

    
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;
    int height, width, rez;
    int numStrips, numTrisPerStrip;

    bool wireframe = false;

};

#endif // TERRAIN_H