#include "application.h"
#include "../resources/resource_manager.h"


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

    ResourceManager::LoadShader("shaders/camera.vs", "shaders/camera.fs", nullptr, "shader");
    shader = ResourceManager::GetShader("shader");

    ResourceManager::LoadShader("shaders/hitbox.vs", "shaders/hitbox.fs", nullptr, "hitbox");
    hitboxShader = ResourceManager::GetShader("hitbox");

    ResourceManager::LoadShader("shaders/lights.vs", "shaders/lights.fs", nullptr, "lights");
    lightShader = ResourceManager::GetShader("lights");

    ResourceManager::LoadShader("shaders/PBR.vs", "shaders/PBR.fs", nullptr, "PBR");
    PBR = ResourceManager::GetShader("PBR");

    myCamera = new Camera(glm::vec3(0.0f, 1.0f, 2.0f));

    //Cube object
    cube = new Cube();
    cube->collisionEnabled = true;
    cube->isStatic = false;
    cube->setPosition(glm::vec3(0.0f, 7.0f, 0.0f));

    plane = new Plane();
    plane->collisionEnabled = true;
    plane->isStatic = true;
    plane->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    primitives.push_back(cube);
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

    light.addPointLight(glm::vec3(-5.0f, 5.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 10.0f);
    
    light.addSpotlight(glm::vec3(5.0f, 5.0f, 5.0f), glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 40.0f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(30.0f)));
}

void Game::Update(float dt)
{
    collision.update(primitives);

    //move the light in a circle 
    float radius = 2.5f;
    float lightX = sin(glfwGetTime()) * radius;
    float lightZ = cos(glfwGetTime()) * radius;
    //spot_light.setPosition(glm::vec3(lightX, 5.0f, lightZ));
    //sphere_light->setPosition(glm::vec3(lightX, 5.0f, lightZ));
}

void Game::Render()
{
    //use light shader
    light.useLight(PBR, *myCamera);

    for (int i = 0; i < primitives.size(); i++)
    {
        primitives[i]->draw(PBR, *myCamera);
    }

}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_MENU)
    {
        float cameraSpeed = 5.f * dt;
        if (this->Keys[GLFW_KEY_W] ){
            myCamera->ProcessKeyboard(Camera_Movement::FORWARD, cameraSpeed);
        }
        if (this->Keys[GLFW_KEY_S]){
            myCamera->ProcessKeyboard(Camera_Movement::BACKWARD, cameraSpeed);
        }
        if (this->Keys[GLFW_KEY_A]){
            myCamera->ProcessKeyboard(Camera_Movement::LEFT, cameraSpeed);
        }
        if (this->Keys[GLFW_KEY_D]){    
            myCamera->ProcessKeyboard(Camera_Movement::RIGHT, cameraSpeed);
        }
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