#include "player.h"
#include <glm/gtx/string_cast.hpp>

Player::Player(glm::vec3 position, glm::vec3 scale, Camera& camera) 
    : Primitives(), camera(&camera) 
{
    this->position = position;
    this->scale = scale;
    this->velocity = glm::vec3(0.0f);
    this->isJumping = false;
    this->collisionEnabled = true;
    this->isStatic = false;
    updateHitbox();
}

void Player::update(float deltaTime) {


    std::cout << "Player Hitbox: " << glm::to_string(hitbox.min) << " " << glm::to_string(hitbox.max) << std::endl;

    //log for player position
    std::cout << "Player Position: " << this->position.x << " " << this->position.y - 2.0f << " " << this->position.z << std::endl;
}

//take window to pass to camera for checking spacebar
void Player::jump(GLFWwindow* window, float deltaTime) {
    //if (!isJumping) {
    //    this->velocity.y = 10.0f; // Adjust jump strength as needed
    //    isJumping = true;
    //}
    camera->ProcessJump(deltaTime, window);
    this->position = camera->Position;
    //update hitbox
    updateHitbox();
}

//move the player
void Player::movePlayer(Camera_Movement direction, float deltaTime) {
    camera->ProcessMovement(direction, deltaTime);
    this->position = camera->Position;
    //log of the player position
    //update hitbox
    updateHitbox();
    //std::cout << "Player Position: " << this->position.x << " " << this->position.y - 2.0f << " " << this->position.z << std::endl;
}

void Player::applyGravity(float deltaTime, glm::vec3 gravity) {
    if (isJumping) {
        this->velocity += gravity * deltaTime;
        this->position += velocity * deltaTime;
    }
    if (position.y <= 0.0f) {  // If the player hits the ground, stop jumping
        position.y = 0.0f;
        velocity.y = 0.0f;
        isJumping = false;
    }
    updateHitbox();
}

void Player::setup() {
    // Setup function intentionally left empty
}

void Player::draw(Shader& shader, Camera& camera) {
    // Draw the player
    // This function is intentionally left empty
}

void Player::drawHitbox(Shader& shader, Camera& camera) {
    // Draw the player's hitbox
    // This function is intentionally left empty
}

std::string Player::getInfo() const {
    return "Player";
}


//update the hitbox of the player
void Player::updateHitbox() {
    hitbox.min = position - (scale / 2.0f);  // Calculate the min point of the bounding box
    hitbox.max = position + (scale / 2.0f);  // Calculate the max point of the bounding box
    //log hitbox
    //std::cout << "Player Hitbox: " << glm::to_string(hitbox.min) << " " << glm::to_string(hitbox.max) << std::endl;
}