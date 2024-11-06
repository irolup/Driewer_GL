#include "application.h"
#include "../resources/resource_manager.h"
#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "../include/stb_image_write.h"
#include "../include/tiny_gltf.h"
#include <unistd.h>
#include <glm/gtx/string_cast.hpp>


Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete myCamera;
    delete cube;
    delete plane;
    delete sphere;
    //for (int i = 0; i < primitives.size(); i++) {
    //    delete primitives[i];
    //}
}

void Game::Init()
{
    glEnable(GL_DEPTH_TEST);

    ResourceManager::LoadShader("shaders/hitbox.vs", "shaders/hitbox.fs", nullptr, "hitbox");
    hitboxShader = ResourceManager::GetShader("hitbox");

    ResourceManager::LoadShader("shaders/lights.vs", "shaders/lights.fs", nullptr, "lights");
    lightShader = ResourceManager::GetShader("lights");

    ResourceManager::LoadShader("shaders/PBR.vs", "shaders/PBR.fs", nullptr, "PBR");
    PBR = ResourceManager::GetShader("PBR");

    ResourceManager::LoadShader("shaders/PBR_notext.vs", "shaders/PBR_notext.fs", nullptr, "PBR_notext");
    PBR_notext = ResourceManager::GetShader("PBR_notext");

    ResourceManager::LoadShader("shaders/fxaa.vs", "shaders/fxaa.fs", nullptr, "fxaa_shader");
    fxaaShader = ResourceManager::GetShader("fxaa_shader");

    ResourceManager::LoadShader("shaders/taa.vs", "shaders/taa.fs", nullptr, "taa");
    taaShader = ResourceManager::GetShader("taa");

    ResourceManager::LoadShader("shaders/msaa.vs", "shaders/msaa.fs", nullptr, "msaa");
    msaaShader = ResourceManager::GetShader("msaa");

    ResourceManager::LoadShader("shaders/smaa.vs", "shaders/smaa.fs", nullptr, "smaa");
    smaaShader = ResourceManager::GetShader("smaa");

    ResourceManager::LoadShader("shaders/height.vs", "shaders/height.fs", nullptr, "height");
    terrainShader = ResourceManager::GetShader("height");

    ResourceManager::LoadShader("shaders/animation.vs", "shaders/animation.fs", nullptr, "animation");
    animationShader = ResourceManager::GetShader("animation");


    antialiasing = new Antialiasing(Width, Height, Antialiasing::Type::NONE);

    //cam with width and height and position
    myCamera = new Camera(Width, Height, glm::vec3(0.0f, 20.0f, 2.0f));

    //myCamera = new Camera(glm::vec3(0.0f, 2.0f, 2.0f));

    //Cube object
    cube = new Cube();
    cube->collisionEnabled = true;
    cube->isStatic = false;
    cube->setPosition(glm::vec3(0.0f, 7.0f, 0.0f));
    cube->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
    primitives.push_back(cube);

    plane = new Plane();
    plane->collisionEnabled = true;
    plane->isStatic = true;
    plane->setScale(glm::vec3(10.0f, 0.0f, 10.0f));
    plane->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    primitives.push_back(plane);
    
    
    //primitives.push_back(sphere);

    drawHitbox = false;

    //for pointlight
    sphere_light = new Sphere();
    sphere_light->collisionEnabled = false;
    sphere_light->isStatic = true;
    sphere_light->setPosition(glm::vec3(-5.0f, 5.0f, 0.0f));
    primitives.push_back(sphere_light);

    //for spotlight
    sphere_light = new Sphere();
    sphere_light->collisionEnabled = false;
    sphere_light->isStatic = true;
    sphere_light->setPosition(glm::vec3(5.0f, 5.0f, 5.0f));
    primitives.push_back(sphere_light);

    //terrain
    terrain = new Terrain(1.0f);

    light.addPointLight(glm::vec3(-5.0f, 5.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 10.0f);
    
    light.addSpotlight(glm::vec3(5.0f, 5.0f, 5.0f), glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 20.0f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(25.0f)));


    //Collsion test
    player = new Player(glm::vec3(0.0f, 20.0f, 2.0f), glm::vec3(1.0f, 2.0f, 1.0f), *myCamera);
    //primitives.push_back(player);

    //vertices for collision detection
    vertices_terrain = terrain->getVertices();


    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::cout << "Current working dir: " << cwd << std::endl;
    } else {
        perror("getcwd() error");
    }

    //make string from ../models/wooden_axe_02_1k.gltf
    //std::string filename = "models/wooden_axe_02_1k.gltf";
    //load lemon
    std::string filename = "models/lemon_1k.gltf";

    if(modelLoader.loadModel(filename.c_str()))
    {
        modelLoader.bindModel();
        std::cout << "Model loaded" << std::endl;
        //load animation
        //modelLoader.loadAnimation(0);
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
    }
    //set position
    modelLoader.position = glm::vec3(0.8f, 0.5f, 0.8f);
    //reduce scale
    //modelLoader.scale = glm::vec3(0.1f, 0.1f, 0.1f);

    //load with assimp
    model_animation = Model("models/michel.fbx");
 
    animation = Animation("models/michel.fbx", &model_animation);
    animator = Animator(&animation);
    

}

void Game::Update(float dt)
{
    collision.update(primitives);
    collision.updatePlayer(player, primitives, vertices_terrain);

    //update player
    player->update(dt);

    //update animation
    animator.UpdateAnimation(dt);

    std::cout << "PLayer colliding with primitives" << collision.getCollisionWithPlayerwithPrimitives() << std::endl;
    std::cout << "PLayer colliding with terrain" << collision.getCollisionWithPlayerwithTerrain() << std::endl;

}

void Game::Render()
{
    //Bind gbuffer here

    if (fxaaActive)
    {
        antialiasing->BindFramebuffer();
    }

    if(texturesActive)
    {
        light.useLight(PBR, *myCamera);
        
        terrain->draw(terrainShader, *myCamera);
        for (int i = 0; i < primitives.size(); i++)
        {
            primitives[i]->draw(PBR, *myCamera);
        }
        modelLoader.drawModel(PBR, *myCamera);

        //animation NEED TO PUT THOSE IN A FUNCTION INSIDE THE MODEL CLASS
        animationShader.Use();
        auto transforms = animator.GetFinalBoneMatrices();
		//for loop transforms
		for (unsigned int i = 0; i < transforms.size(); i++){
			animationShader.SetMatrix4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
		}
        model_animation.Draw(animationShader, *myCamera);
        
    }
    else
    {
        light.useLight(PBR_notext, *myCamera);
        for (int i = 0; i < primitives.size(); i++)
        {
            primitives[i]->draw(PBR_notext, *myCamera);
        }
    }

    //render SSGI here with the RenderWithShaderfunction

    if (fxaaActive)
    {
        antialiasing->UpdateHistoryBuffer(*myCamera);
        //antialiasing->RenderWithShader(taaShader, *myCamera);
        antialiasing->RenderWithShader(fxaaShader, *myCamera);
        //antialiasing->RenderWithShader(smaaShader, *myCamera);
    }

}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_MENU)
    {
        float cameraSpeed = 4.0f * dt;
        //myCamera->ProcessJump(dt, window);
        if (this->Keys[GLFW_KEY_W] ){
            //myCamera->ProcessKeyboard(Camera_Movement::FORWARD, cameraSpeed);
            player->movePlayer(Camera_Movement::FORWARD, cameraSpeed);
        }
        if (this->Keys[GLFW_KEY_S]){
            //myCamera->ProcessKeyboard(Camera_Movement::BACKWARD, cameraSpeed);
            player->movePlayer(Camera_Movement::BACKWARD, cameraSpeed);
        }
        if (this->Keys[GLFW_KEY_A]){
            //myCamera->ProcessKeyboard(Camera_Movement::LEFT, cameraSpeed);
            player->movePlayer(Camera_Movement::LEFT, cameraSpeed);
        }
        if (this->Keys[GLFW_KEY_D]){    
            //myCamera->ProcessKeyboard(Camera_Movement::RIGHT, cameraSpeed);
            player->movePlayer(Camera_Movement::RIGHT, cameraSpeed);
        }
        //spacebar to jump
        if (this->Keys[GLFW_KEY_SPACE])
        {
            player->jump(window, dt, terrain, vertices_terrain, collision);
        }
        
        //player->jump(window, dt, terrain, vertices_terrain);

        //activated mouse
        if (this->Keys[GLFW_KEY_M]){
            isMouseEnabled = true;
            EnableMouse();
            std::cout << "Mouse is enabled" << std::endl;
        }
        if (this->Keys[GLFW_KEY_N]){
            isMouseEnabled = false;
            DisableMouse();
            std::cout << "Mouse is disabled" << std::endl;
        }
        //t and y to texture and no texture
        if (this->Keys[GLFW_KEY_T])
        {
            texturesActive = true;
            std::cout << "Textures are enabled" << std::endl;
        }
        if (this->Keys[GLFW_KEY_Y])
        {
            texturesActive = false;
            std::cout << "Textures are disabled" << std::endl;
        }
        

        //key c to make all primitives static
        if (this->Keys[GLFW_KEY_C]){
            for (int i = 0; i < primitives.size(); i++) {
                primitives[i]->isStatic = true;
            }
            std::cout << "All primitives are static" << std::endl;
        }
        //key x to make all primitives dynamic (not the plane)
        if (this->Keys[GLFW_KEY_X]){
            for (int i = 0; i < primitives.size(); i++) {
                if (primitives[i]->getInfo() != "Plane") {
                    primitives[i]->isStatic = false;
                } else {
                    primitives[i]->isStatic = true;
                }
            }
            std::cout << "All primitives are dynamic" << std::endl;
        }
        //key z to replace the primitives to their original position
        if (this->Keys[GLFW_KEY_Z]){
            for (int i = 0; i < primitives.size(); i++) {
                if (primitives[i]->getInfo() == "Plane") {
                    primitives[i]->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
                } else if //sphere not change position
                (primitives[i]->getInfo() == "Sphere") {
                    primitives[i]->setPosition(glm::vec3(0.0f, 9.0f, 0.0f));
                } else if (primitives[i]->getInfo() == "Cube") {
                    primitives[i]->setPosition(glm::vec3(0.0f, 3.0f, 0.0f));
                }
            }
            std::cout << "All primitives are back to their original position" << std::endl;
        }
        //key h to draw hitbox and j to not draw hitbox
        if (this->Keys[GLFW_KEY_H]){
            drawHitbox = true;
            std::cout << "Hitbox is enabled" << std::endl;
        }
        if (this->Keys[GLFW_KEY_J]){
            drawHitbox = false;
            std::cout << "Hitbox is disabled" << std::endl;
        }
        //1 to enable taa and 2 to disable fxaa
        if (this->Keys[GLFW_KEY_1]){
            fxaaActive = true;
            std::cout << "FXAA is enabled" << std::endl;
        }
        if (this->Keys[GLFW_KEY_2]){
            fxaaActive = false;
            std::cout << "FXAA is disabled" << std::endl;
        }

    }
    if (this->State == GAME_WIN)
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->State = GAME_MENU;
        }
    }
    if (this->State == GAME_ACTIVE)
    {
        
    }
}

void Game::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (!isMouseEnabled)
    {
        return;
    }
    
    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    // Calculate the offset between the current and previous positions
    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos); // Reversed since y-coordinates go from bottom to top

    // Update lastX and lastY to the current position
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    // Pass the offsets to the camera (be sure this method handles both offsets)
    myCamera->ProcessMouseMovement(xoffset, yoffset);
}

void Game::SetWindow(GLFWwindow* win) {
    window = win;
}

void Game::EnableMouse() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    isMouseEnabled = true;
}

void Game::DisableMouse() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    isMouseEnabled = false;
}

void Game::SetCameraWindowSize(unsigned int width, unsigned int height) {
    myCamera->Width = width;
    myCamera->Height = height;
    antialiasing->Update(width, height);
}