#ifndef COLLISION_H
#define COLLISION_H

#include "../primitives/primitives.h"
#include "../player/player.h"
#include <vector>
#include <glm/glm.hpp>

class Collision {
public:
    glm::vec3 gravity;  // Gravity vector (e.g., glm::vec3(0.0f, -9.81f, 0.0f))
    float deltaTime;     // Time step for simulation

    // Constructor to initialize gravity and deltaTime
    Collision(glm::vec3 g = glm::vec3(0.0f, -9.81f, 0.0f), float dt = 0.016f);

    // Check for collision between two primitives (simplified AABB bounding box collision)
    bool checkCollision(Primitives* a, Primitives* b);

    // Resolve collision between two primitives by modifying their velocities
    void resolveCollision(Primitives* a, Primitives* b);

    // Apply gravity to a primitive, modifying its velocity and position
    void applyGravity(Primitives* primitive);

    // Update method to apply gravity and resolve collisions between all primitives
    void update(std::vector<Primitives*>& primitives);
    void updateWithPLayer(std::vector<Primitives*>& primitives, Player* player);

private:

    glm::vec3 calculateNormal(Primitives* a, Primitives* b);
    void slideAlongSurface(Primitives* moving, glm::vec3 normal);
    void handlePlayerCollision(Player* player, std::vector<Primitives*>& primitives);

};

#endif // COLLISION_H