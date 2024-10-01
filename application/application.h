#ifndef GAME_H
#define GAME_H

#include "../glad/glad.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <tuple>
#include <algorithm>
#include <sstream>
#include "../include/stb_image.h"
#include "../camera/camera.h"
#include "../primitives/primitives.h"
#include "../primitives/cube.h"
#include "../primitives/sphere.h"
#include "../primitives/plane.h"
#include "../collision/collision.h"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState               State;
    // Game-related State data

    Camera          *myCamera;
    Shader          shader;
    Shader          hitboxShader;
    Cube* cube;
    Plane* plane;
    Sphere* sphere;
    std::vector<Primitives*> primitives;
    Collision collision;

    bool                    Keys[1024];
    bool                    KeysProcessed[1024];
    unsigned int            Width, Height;
    bool firstMouse = true;
    float lastX = Width / 2.0f;
    float lastY = Height / 2.0f;

    bool drawHitbox = false;

    //audio

    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    void Update(float dt);
    void Render();

    //Collision detection
    
    private:


};

#endif