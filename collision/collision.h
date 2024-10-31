#ifndef COLLISION_H
#define COLLISION_H

#include "../primitives/primitives.h"
#include "../player/player.h"
#include "../world_objects/terrain.h"
#include <vector>
#include <glm/glm.hpp>

class Player;  // Forward declaration

class Collision {
public:
    glm::vec3 gravity;  // Gravity vector (e.g., glm::vec3(0.0f, -9.81f, 0.0f))
    float deltaTime;     // Time step for simulation
    //terrain
    Terrain* terrain;

    // Constructor to initialize gravity and deltaTime
    Collision(glm::vec3 g = glm::vec3(0.0f, -9.81f, 0.0f), float dt = 0.016f);

    // Check for collision between two primitives (simplified AABB bounding box collision)
    bool checkCollision(Primitives* a, Primitives* b);

    // Resolve collision between two primitives by modifying their velocities
    void resolveCollision(Primitives* a, Primitives* b);

    // Apply gravity to a primitive, modifying its velocity and position
    void applyGravity(Primitives* primitive);
    void applyGravity(Player* player);

    // Update method to apply gravity and resolve collisions between all primitives
    void update(std::vector<Primitives*>& primitives);



    // Function to check and resolve player collisions
    bool checkPlayerCollision(Player* player, Primitives* primitive);
    void resolvePlayerCollision(Player* player, Primitives* primitive);
    void updatePlayer(Player* player, std::vector<Primitives*>& primitives, std::vector<glm::vec3> vertices);

    bool checkPlayerTerrainCollision(Player* player, Terrain* terrain, std::vector<glm::vec3> vertices);


    bool getCollisionWithPlayerwithPrimitives();
    bool getCollisionWithPlayerwithTerrain();

private:

    glm::vec3 calculateNormal(Primitives* a, Primitives* b);
    void slideAlongSurface(Primitives* moving, glm::vec3 normal);
    bool PlayerCollidingWithPrimitives;
    bool PlayerCollidingWithTerrain;
};

#endif // COLLISION_H