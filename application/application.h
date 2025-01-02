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
#include "../world_objects/terrain.h"
#include "../collision/collision.h"
#include "../lights/lights.h"
#include "antialiasing.h"
#include "../models/modelLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../models/assimp/animator.h"
#include "../models/assimp/model_animation.h"
#include "gbuffer.h"
#include "ssaobuffer.h"
#include "../lights/shadows.h"

#include "../include/imgui/imgui.h"
#include "../include/imgui/backends/imgui_impl_glfw.h"
#include "../include/imgui/backends/imgui_impl_opengl3.h"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

//Render mode foward or deferred
enum RenderMode {
    FORWARD_RENDERING,
    DEFERRED_RENDERING,
    OTHER,
    SHADOWS
};

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState               State;

    //render mode
    RenderMode              Rendermode;
    // Game-related State data

    Player          *player;
    Camera          *myCamera;
    Shader          hitboxShader;
    Shader          lightShader;
    Shader          PBR;
    Shader          PBR_notext;
    Shader          fxaaShader;
    Shader          defaultShader;
    //taa shader
    Shader          taaShader;
    Shader          msaaShader;
    Shader          smaaShader;
    //terrain shader
    Shader          terrainShader;
    Antialiasing*   antialiasing;
    GBuffer*        GBuffer_;
    //gbufer shader
    Shader          Gbuffer_shader;
    Shader          lightpass;
    Shader          ssaoshader;
    Shader          ssaoblurshader;
    Shader          lightpassSSAO;
    Shader          simpleDepthShader;
    Shader          pbr_shadows;

    Cube* cube;
    Plane* plane;
    Sphere* sphere;
    Terrain* terrain;
    std::vector<glm::vec3> vertices_terrain;

    //cube light
    Sphere* sphere_light;
    float light_width;
    float light_height;

    std::vector<Primitives*> primitives;

    Light light;
    Shadows shadows;
    
    //vector of lights
    std::vector<Light*> lights;

    
    Collision collision;

    ModelLoader modelLoader;
    tinygltf::Model model_glb;

    Model model_animation;
    Animation animation;
    Animator animator;
    Shader animationShader;

    glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f); // Gravity force
    float deltaTime = 0.016f;

    bool                    Keys[1024];
    bool                    KeysProcessed[1024];
    unsigned int            Width, Height;
    bool firstMouse = true;
    float lastX = Width / 2.0f;
    float lastY = Height / 2.0f;

    bool isMouseEnabled = true;
    void EnableMouse();
    void DisableMouse();

    GLFWwindow* window;

    void SetCameraWindowSize(unsigned int width, unsigned int height);

    bool drawHitbox = false;
    bool texturesActive = true;
    bool fxaaActive = false;

    //imgui
    bool mainwindow = true;
    float sliderx = 0.0f;
    float sliderz = 0.0f;

    float ao = 0.0f;
    float aoSlider = 0.0f;
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
    void SetWindow(GLFWwindow* window);
    void cleanup();

    //Collision detection
    
    private:
        ssaoBuffer* ssao;

};

#endif