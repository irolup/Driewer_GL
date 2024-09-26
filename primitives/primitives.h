#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "../shaders/shader.h"

class Primitives {
public:
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec4 color;

    // PBR Material properties (as an example)
    struct Material {
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        glm::vec4 emission;
        float shininess;
    } material;

    Primitives(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 scl = glm::vec3(1.0f), glm::vec4 clr = glm::vec4(1.0f)) 
        : position(pos), scale(scl), color(clr) {
            material.ambient = glm::vec4(1.0f);
            material.diffuse = glm::vec4(0.5f);
            material.specular = glm::vec4(1.0f);
            material.emission = glm::vec4(0.0f);
            material.shininess = 32.0f; // Default shininess
        }

    //setup function
    virtual void setup() = 0;

    // Pure virtual function for drawing
    virtual void draw(Shader& shader, glm::vec3 position, glm::vec3 cameraPos) = 0;

    // Get information about the primitive
    virtual std::string getInfo() const = 0;

    // Set the color of the primitive
    void setColor(const glm::vec4& clr) {
        color = clr;
    }

    // Set the scale of the primitive
    void setScale(const glm::vec3& scl) {
        scale = scl;
    }

    virtual ~Primitives() {}
};

#endif // PRIMITIVES_H