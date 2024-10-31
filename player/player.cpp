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

    //update position for the camera
    camera->Position = this->position;

    //log for player position
    //std::cout << "Player Position: " << this->position.x << " " << this->position.y - 2.0f << " " << this->position.z << std::endl;
}

//take window to pass to camera for checking spacebar
void Player::jump(GLFWwindow* window, float deltaTime, Terrain* terrain, std::vector<glm::vec3> vertices, Collision &collision) {
    
        // Only allow jumping if the player is not already jumping and is on the ground
        //if colliding with terrain or primitives
        
        if (collision.getCollisionWithPlayerwithTerrain() || collision.getCollisionWithPlayerwithPrimitives()) {
            this->velocity.y = 10.0f; // Adjust jump strength as needed
            isJumping = true; // Set jumping state
            camera->ProcessJump(deltaTime, window);
            updateHitbox();
        }
    
    
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
    updateHitbox();
}

void Player::setup() {
    // Setup function intentionally left empty
}

void Player::draw(Shader& shader, Camera& camera) {
    // Draw the player
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