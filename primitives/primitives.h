#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "../shaders/shader.h"
#include "../camera/camera.h"
#include "geometryUtils.h"

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
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float metallic;
        float roughness;
        float occlusion;
        float brightness;
        glm::vec3 fresnel_ior;
    } material;

    //struct for hitbox
    struct Hitbox {
        glm::vec3 min;
        glm::vec3 max;
    } hitbox;

    Primitives(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 scl = glm::vec3(1.0f), glm::vec4 clr = glm::vec4(1.0f),
        bool moving = false, bool collision = false, float m = 1.0f, glm::vec3 vel = glm::vec3(0.0f)) 
        : position(pos), scale(scl), color(clr), isStatic(moving), collisionEnabled(collision), mass(m), velocity(vel) {
            material.ambient = glm::vec3(1.0f);
            material.diffuse = glm::vec3(0.5f);
            material.specular = glm::vec3(1.0f);
            material.metallic = 0.0f;
            material.roughness = 0.5f;
            material.occlusion = 1.0f;
            material.brightness = 1.0f;
            material.fresnel_ior = glm::vec3(1.5f);
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