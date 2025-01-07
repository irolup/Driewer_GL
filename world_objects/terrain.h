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
    Terrain(float gridSize);
    void draw(Shader& shader, Camera& camera) override;
    void drawWithShadow(Shader& shader, Camera& camera, unsigned int depthMap, bool pointLight = false) override;

    std::vector<glm::vec3> vertices;

    //function to return the indices positions forcollision detection
    std::vector<glm::vec3> getVertices();

    float getGridSize() const;
    void setGridSize(float gridSize);
    float getHeightAt(float x, float z);
    int getTerrainWidth() const;
    int getTerrainHeight() const;
    
private:
    unsigned int VAO, VBO, EBO;
    unsigned int texture_diffuse, texture_normal, texture_metalllic, texture_roughness, texture_ao, texture_disp;
    unsigned int heightmap;

    std::vector<unsigned int> textures;

    void setup();
    void loadTextures();
    void generateTerrain(float gridSize = 1.0f);

    std::string getInfo() const override;

    
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;
    int height, width;
    float rez;
    int numStrips, numTrisPerStrip;

    bool wireframe = false;

    float gridSize = 1.0f;

};

#endif // TERRAIN_H