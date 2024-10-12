#pragma once
#include <glm/glm.hpp>
#include "../camera/camera.h"
#include "../primitives/primitives.h"
#include "../shaders/shader.h"
#include <string>

class Player : public Primitives {
public:
    glm::vec3 position;          // Player's position
    glm::vec3 scale;            // Player's scale
    glm::vec3 velocity;         // Player's velocity
    bool isJumping;             // Track if the player is in the air
    bool collisionEnabled;       // Check if collision is enabled
    bool isStatic;              // Check if the player is static
    Camera* camera;             // Player's camera


    Player(glm::vec3 position, glm::vec3 scale, Camera& camera); // Constructor

    void update(float deltaTime);  // Update player's position
    void jump(GLFWwindow* window, float deltaTime); // Jump function
    void applyGravity(float deltaTime, glm::vec3 gravity); // Apply gravity

    virtual void setup() override; // Setup function do nothing

    virtual void draw(Shader& shader, Camera& camera) override; // Draw function do nothing

    virtual void drawHitbox(Shader& shader, Camera& camera) override; // Draw hitbox function do nothing

    virtual std::string getInfo() const override; // Get information about the player

    void movePlayer(Camera_Movement direction, float deltaTime); // Move the player

    void movePlayer(); // Move the player
};