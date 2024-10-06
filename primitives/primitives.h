#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "../shaders/shader.h"
#include "../camera/camera.h"

class Primitives {
public:
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec4 color;
    glm::vec3 velocity;
    float mass;
    bool isStatic;
    bool collisionEnabled;

    // PBR Material properties (as an example)
    struct Material {
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        glm::vec4 emission;
        float shininess;
    } material;

    //struct for hitbox
    struct Hitbox {
        glm::vec3 min;
        glm::vec3 max;
    } hitbox;

// couleurs de réflexion du matériau
//uniform vec3 material_color_ambient;
//uniform vec3 material_color_diffuse;
//uniform vec3 material_color_specular;
//
//// facteur de brillance du matériau
//uniform float material_brightness;
//
//// facteur de métallicité du matériau
//uniform float material_metallic;
//
//// facteur de rugosité du matériau
//uniform float material_roughness;
//
//// facteur d'occlusion ambiante
//uniform float material_occlusion;
//
//// indice de réfraction de l'effet de Fresnel;
//uniform vec3 material_fresnel_ior;


    Primitives(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 scl = glm::vec3(1.0f), glm::vec4 clr = glm::vec4(1.0f),
        bool moving = false, bool collision = false, float m = 1.0f, glm::vec3 vel = glm::vec3(0.0f)) 
        : position(pos), scale(scl), color(clr), isStatic(moving), collisionEnabled(collision), mass(m), velocity(vel) {
            material.ambient = glm::vec4(1.0f);
            material.diffuse = glm::vec4(0.5f);
            material.specular = glm::vec4(1.0f);
            material.emission = glm::vec4(0.0f);
            material.shininess = 32.0f; // Default shininess
            updateHitbox();
        }

    //setup function
    virtual void setup() = 0;

    // Pure virtual function for drawing
    virtual void draw(Shader& shader, Camera& camera) = 0;

    // Pure virtual function for drawing the hitbox
    virtual void drawHitbox(Shader& shader, Camera& camera) = 0;

    // Get information about the primitive
    virtual std::string getInfo() const = 0;

    void updateHitbox() {
        hitbox.min = position - (scale / 2.0f);  // Calculate the min point of the bounding box
        hitbox.max = position + (scale / 2.0f);  // Calculate the max point of the bounding box
    }

    // Set the color of the primitive
    void setColor(const glm::vec4& clr) {
        color = clr;
    }

    // Set the scale of the primitive
    void setScale(const glm::vec3& scl) {
        scale = scl;
        updateHitbox();
    }

    //Set the position of the primitive
    void setPosition(const glm::vec3& pos) {
        position = pos;
        updateHitbox();
    }

    // Set the velocity of the primitive
    void setVelocity(const glm::vec3& vel) {
        velocity = vel;
    }

    // Set the mass of the primitive
    void setMass(float m) {
        mass = m;
    }

    // Set the static property of the primitive
    void setStatic(bool moving) {
        isStatic = moving;
    }

    //get hitbox
    Hitbox getHitbox() const {
        return hitbox;
    }

    //Get hibox position
    glm::vec3 getHitboxPosition() const {
        return (hitbox.min + hitbox.max) / 2.0f;
    }

    //get position
    glm::vec3 getPosition() const {
        return position;
    }

    virtual ~Primitives() {}
};

#endif // PRIMITIVES_H