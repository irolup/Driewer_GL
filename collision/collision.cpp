#include "collision.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

// Constructor to initialize gravity and time step
Collision::Collision(glm::vec3 g, float dt) : gravity(g), deltaTime(dt) {}

// Check for a collision between two primitives (using Axis-Aligned Bounding Box - AABB)
bool Collision::checkCollision(Primitives* a, Primitives* b) {
    if (!a->collisionEnabled || !b->collisionEnabled) {
        return false;  // If collision is disabled for either object, no collision
    }

    // Update hitboxes for collision detection
    a->updateHitbox();
    b->updateHitbox();

    // Use the updated hitboxes to check for overlap
    glm::vec3 aMin = a->hitbox.min;
    glm::vec3 aMax = a->hitbox.max;
    glm::vec3 bMin = b->hitbox.min;
    glm::vec3 bMax = b->hitbox.max;

    // Check for overlap
    bool collisionDetected = (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
                             (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
                             (aMin.z <= bMax.z && aMax.z >= bMin.z);

    // Debugging output
    if (collisionDetected) {
        std::cout << "Collision detected between: " << a->getInfo() << " and " << b->getInfo() << std::endl;
    }
    return collisionDetected;
}

// Calculate the collision normal based on the positions of the two primitives
glm::vec3 Collision::calculateNormal(Primitives* a, Primitives* b) {
    // Use the centers of the hitboxes to calculate the collision normal
    glm::vec3 aCenter = (a->hitbox.min + a->hitbox.max) * 0.5f;
    glm::vec3 bCenter = (b->hitbox.min + b->hitbox.max) * 0.5f;
    return glm::normalize(aCenter - bCenter);
}

// Slide the moving primitive along the surface of the other
void Collision::slideAlongSurface(Primitives* moving, glm::vec3 normal) {
    float dotProduct = glm::dot(moving->velocity, normal);
    if (dotProduct < 0) { // Only slide if moving towards the surface
        glm::vec3 slideVelocity = moving->velocity - (dotProduct * normal);
        moving->velocity = slideVelocity;
    }
}

// Resolve the collision by separating the objects and adjusting their velocities
void Collision::resolveCollision(Primitives* a, Primitives* b) {
    if (!checkCollision(a, b)) return;

    glm::vec3 overlap;
    // Use the hitboxes for calculations
    glm::vec3 aMin = a->hitbox.min;
    glm::vec3 aMax = a->hitbox.max;
    glm::vec3 bMin = b->hitbox.min;
    glm::vec3 bMax = b->hitbox.max;

    // Compute overlap on each axis
    overlap.x = std::min(aMax.x, bMax.x) - std::max(aMin.x, bMin.x);
    overlap.y = std::min(aMax.y, bMax.y) - std::max(aMin.y, bMin.y);
    overlap.z = std::min(aMax.z, bMax.z) - std::max(aMin.z, bMin.z);

    glm::vec3 normal = calculateNormal(a, b);

    // Check if A is static and B is dynamic
    if (a->isStatic && !b->isStatic) {
        if (overlap.y < overlap.x && overlap.y < overlap.z) {
            b->position.y = aMax.y + b->scale.y / 2.0f; // Move B out of A
            b->velocity.y = 0.0f; // Stop downward movement
            slideAlongSurface(b, normal); // Slide B along the surface
        } else if (overlap.x < overlap.z) {
            b->position.x += (aMax.x < b->position.x) ? overlap.x : -overlap.x;
            slideAlongSurface(b, normal);
        } else {
            b->position.z += (aMax.z < b->position.z) ? overlap.z : -overlap.z;
            slideAlongSurface(b, normal);
        }
    } else if (!a->isStatic && b->isStatic) {
        if (overlap.y < overlap.x && overlap.y < overlap.z) {
            a->position.y = bMax.y + a->scale.y / 2.0f; // Move A out of B
            a->velocity.y = 0.0f; // Stop upward movement
            slideAlongSurface(a, normal);
        } else if (overlap.x < overlap.z) {
            a->position.x += (bMax.x < a->position.x) ? overlap.x : -overlap.x;
            slideAlongSurface(a, normal);
        } else {
            a->position.z += (bMax.z < a->position.z) ? overlap.z : -overlap.z;
            slideAlongSurface(a, normal);
        }
    } else if (!a->isStatic && !b->isStatic) {
    // Handle both objects being dynamic
    // Separate the objects and adjust their velocities appropriately
    glm::vec3 correction = normal * overlap;
    a->position -= correction * 0.5f;
    b->position += correction * 0.5f;
    
    slideAlongSurface(a, normal);
    slideAlongSurface(b, normal);
    }
}

// Apply gravity to a primitive (modifying its velocity and position)
void Collision::applyGravity(Primitives* primitive) {
    if (primitive->collisionEnabled && !primitive->isStatic) {
        // Gravity affects the primitive's velocity over time if it's dynamic
        primitive->velocity += gravity * deltaTime;
        primitive->position += primitive->velocity * deltaTime;
        //std::cout << "Gravity applied to: " << primitive->getInfo() << " | New position: " << glm::to_string(primitive->position) << std::endl;
    }
}



// Update function: Apply gravity and check collisions between all primitives
void Collision::update(std::vector<Primitives*>& primitives) {
    // Apply gravity to all primitives first
    for (size_t i = 0; i < primitives.size(); ++i) {
        applyGravity(primitives[i]);
    }

    // Then resolve collisions
    for (size_t i = 0; i < primitives.size(); ++i) {
        for (size_t j = i + 1; j < primitives.size(); ++j) {
            resolveCollision(primitives[i], primitives[j]);
        }
    }
}

bool Collision::checkPlayerCollision(Player* player, Primitives* primitive) {
    if (!player->collisionEnabled || !primitive->collisionEnabled) {
        return false;  // If collision is disabled for either object, no collision
    }

    // Update hitboxes for collision detection
    player->updateHitbox();
    primitive->updateHitbox();

    // Use the updated hitboxes to check for overlap
    glm::vec3 playerMin = player->hitbox.min;
    glm::vec3 playerMax = player->hitbox.max;
    glm::vec3 primMin = primitive->hitbox.min;
    glm::vec3 primMax = primitive->hitbox.max;

    // Check for overlap
    bool collisionDetected = (playerMin.x <= primMax.x && playerMax.x >= primMin.x) &&
                             (playerMin.y <= primMax.y && playerMax.y >= primMin.y) &&
                             (playerMin.z <= primMax.z && playerMax.z >= primMin.z);

    // Debugging output
    if (collisionDetected) {
        std::cout << "Collision detected between Player and " << primitive->getInfo() << std::endl;
    }
    return collisionDetected;
}

// Resolve the collision between the player and a primitive
void Collision::resolvePlayerCollision(Player* player, Primitives* primitive) {
    if (!checkPlayerCollision(player, primitive)) return;

    glm::vec3 overlap;
    // Use the hitboxes for calculations
    glm::vec3 playerMin = player->hitbox.min;
    glm::vec3 playerMax = player->hitbox.max;
    glm::vec3 primMin = primitive->hitbox.min;
    glm::vec3 primMax = primitive->hitbox.max;

    // Compute overlap on each axis
    overlap.x = std::min(playerMax.x, primMax.x) - std::max(playerMin.x, primMin.x);
    overlap.y = std::min(playerMax.y, primMax.y) - std::max(playerMin.y, primMin.y);
    overlap.z = std::min(playerMax.z, primMax.z) - std::max(playerMin.z, primMin.z);

    glm::vec3 normal = calculateNormal(player, primitive);

    if (overlap.y < overlap.x && overlap.y < overlap.z) {
        player->position.y = primMax.y + player->scale.y / 2.0f;  // Move player out of primitive
        player->velocity.y = 0.0f; // Stop downward movement
        slideAlongSurface(player, normal); // Slide the player along the surface
    } else if (overlap.x < overlap.z) {
        player->position.x += (primMax.x < player->position.x) ? overlap.x : -overlap.x;
        slideAlongSurface(player, normal);
    } else {
        player->position.z += (primMax.z < player->position.z) ? overlap.z : -overlap.z;
        slideAlongSurface(player, normal);
    }
}

// Update function: Apply gravity and check collisions between the player and all primitives
void Collision::updatePlayer(Player* player, std::vector<Primitives*>& primitives) {
    // Apply gravity to the player
    //applyGravity(player);

    // Then resolve collisions with primitives
    for (size_t i = 0; i < primitives.size(); ++i) {
        resolvePlayerCollision(player, primitives[i]);
    }
}

// Update gravity for player (added to support gravity application)
void Collision::applyGravity(Player* player) {
    if (player->collisionEnabled && !player->isStatic) {
        // Gravity affects the player's velocity over time if they're dynamic
        player->velocity += gravity * deltaTime;
        player->position += player->velocity * deltaTime;
        // Update the player's hitbox
        player->updateHitbox();
    }
}