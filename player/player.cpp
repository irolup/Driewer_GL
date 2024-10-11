#include "player.h"

Player::Player(glm::vec3 position, glm::vec3 scale, Camera& camera) 
    : camera(&camera) 
{
    this->position = position;
    this->scale = scale;
    this->velocity = glm::vec3(0.0f);
    this->isJumping = false;
    this->collisionEnabled = true;
    this->isStatic = false;
}

void Player::update(float deltaTime) {
    // Update the player's position based on velocity and deltaTime
    //this->position += this->velocity * deltaTime;
    // Update the player's camera position
    //camera->Position = this->position;

    //take the camera position and put the camera position for the player position
    this->position = camera->Position;
    //log for player position
    std::cout << "Player Position: " << this->position.x << " " << this->position.y - 2.0f << " " << this->position.z << std::endl;
}

void Player::jump() {
    if (!isJumping) {
        this->velocity.y = 10.0f; // Adjust jump strength as needed
        isJumping = true;
    }
}

void Player::applyGravity(float deltaTime, glm::vec3 gravity) {
    if (isJumping) {
        this->velocity += gravity * deltaTime;
    }
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

//function to move the player
void Player::movePlayer(){
    position = camera->Position;
}

