#pragma once
#include <glm/glm.hpp>
#include "../collision/collision.h"
#include "../world_objects/terrain.h"
#include "../camera/camera.h"
#include "../primitives/primitives.h"
#include "../shaders/shader.h"
#include <string>
#include "../include/stb_image.h"

class Collision; // Forward declaration

class Player : public Primitives {
public:
    glm::vec3 position;          // Player's position
    glm::vec3 scale;            // Player's scale
    glm::vec3 velocity;         // Player's velocity
    bool isJumping;             // Track if the player is in the air
    bool collisionEnabled;       // Check if collision is enabled
    bool isStatic;              // Check if the player is static
    Camera* camera;             // Player's camera
    Terrain* terrain;           // Terrain object


    Player(glm::vec3 position, glm::vec3 scale, Camera& camera); // Constructor

    void update(float deltaTime);  // Update player's position
    void jump(GLFWwindow* window, float deltaTime, Terrain* terrain, std::vector<glm::vec3> vertices, Collision &collision); // Jump function
    void applyGravity(float deltaTime, glm::vec3 gravity); // Apply gravity

    virtual void setup() override; // Setup function do nothing

    virtual void draw(Shader& shader, Camera& camera) override; // Draw function do nothing
    virtual void drawWithShadow(Shader& shader, Camera& camera, unsigned int depthMap) override; // Draw with shadow function do nothing
    void drawTest(Shader& shader, Camera& camera) override;

    virtual std::string getInfo() const override; // Get information about the player

    void movePlayer(Camera_Movement direction, float deltaTime); // Move the player

    void updateHitbox(); // Update the player's hitbox
};