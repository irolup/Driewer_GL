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

    // Calculate bounding boxes
    glm::vec3 aMin = a->position - a->scale / 2.0f;
    glm::vec3 aMax = a->position + a->scale / 2.0f;
    glm::vec3 bMin = b->position - b->scale / 2.0f;
    glm::vec3 bMax = b->position + b->scale / 2.0f;

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

// Resolve the collision by separating the objects and adjusting their velocities
void Collision::resolveCollision(Primitives* a, Primitives* b) {
    if (!checkCollision(a, b)) return;

    glm::vec3 overlap;
    glm::vec3 aMin = a->position - a->scale / 2.0f;
    glm::vec3 aMax = a->position + a->scale / 2.0f;
    glm::vec3 bMin = b->position - b->scale / 2.0f;
    glm::vec3 bMax = b->position + b->scale / 2.0f;

    // Compute overlap on each axis
    overlap.x = std::min(aMax.x, bMax.x) - std::max(aMin.x, bMin.x);
    overlap.y = std::min(aMax.y, bMax.y) - std::max(aMin.y, bMin.y);
    overlap.z = std::min(aMax.z, bMax.z) - std::max(aMin.z, bMin.z);

    if (a->isStatic && !b->isStatic) {
        // Static A, Dynamic B
        // Resolve collision by adjusting B's position
        if (overlap.y < overlap.x && overlap.y < overlap.z) {
            b->position.y = aMax.y + b->scale.y / 2.0f; // Move B out of A
            b->velocity.y = 0.0f;  // Stop downward movement
            std::cout << "Resolved collision on Y-axis: " << b->getInfo() << " and " << a->getInfo() << std::endl;
        } else if (overlap.x < overlap.z) {
            b->position.x += (aMax.x < b->position.x) ? overlap.x : -overlap.x;
            std::cout << "Resolved collision on X-axis: " << b->getInfo() << " and " << a->getInfo() << std::endl;
        } else {
            b->position.z += (aMax.z < b->position.z) ? overlap.z : -overlap.z;
            std::cout << "Resolved collision on Z-axis: " << b->getInfo() << " and " << a->getInfo() << std::endl;
        }
        b->velocity = glm::vec3(0.0f);  // Stop all velocity upon collision
    } else if (!a->isStatic && b->isStatic) {
        // Dynamic A, Static B
        // Similar logic as above, ensure only A is modified
        if (overlap.y < overlap.x && overlap.y < overlap.z) {
            a->position.y = bMax.y + a->scale.y / 2.0f; // Move A out of B
            a->velocity.y = 0.0f;
            std::cout << "Resolved collision on Y-axis: " << a->getInfo() << " and " << b->getInfo() << std::endl;
        } else if (overlap.x < overlap.z) {
            a->position.x += (bMax.x < a->position.x) ? overlap.x : -overlap.x;
            std::cout << "Resolved collision on X-axis: " << a->getInfo() << " and " << b->getInfo() << std::endl;
        } else {
            a->position.z += (bMax.z < a->position.z) ? overlap.z : -overlap.z;
            std::cout << "Resolved collision on Z-axis: " << a->getInfo() << " and " << b->getInfo() << std::endl;
        }
        a->velocity = glm::vec3(0.0f);
    }
    // Handle the case where both A and B are dynamic if needed
}

// Apply gravity to a primitive (modifying its velocity and position)
void Collision::applyGravity(Primitives* primitive) {
    if (primitive->collisionEnabled && !primitive->isStatic) {
        // Gravity affects the primitive's velocity over time if it's dynamic
        primitive->velocity += gravity * deltaTime;
        primitive->position += primitive->velocity * deltaTime;
        std::cout << "Gravity applied to: " << primitive->getInfo() << " | New position: " << glm::to_string(primitive->position) << std::endl;
    }
}

// Update function: Apply gravity and check collisions between all primitives
void Collision::update(std::vector<Primitives*>& primitives) {
    for (size_t i = 0; i < primitives.size(); ++i) {
        // Apply gravity to each primitive
        applyGravity(primitives[i]);

        // Check for collisions with other primitives
        for (size_t j = i + 1; j < primitives.size(); ++j) {
            resolveCollision(primitives[i], primitives[j]);
        }
    }
}