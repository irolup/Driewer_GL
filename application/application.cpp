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

    myCamera = new Camera(glm::vec3(0.0f, 1.0f, 2.0f));

    //Cube object
    cube = new Cube();
    cube->collisionEnabled = true;
    cube->isStatic = false;
    cube->setPosition(glm::vec3(0.0f, 5.0f, 0.0f));

    plane = new Plane();
    plane->collisionEnabled = true;
    plane->isStatic = true;
    plane->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    //sphere = new Sphere();
    //sphere->collisionEnabled = true;
    //sphere->isStatic = false;
    //sphere->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));

    primitives.push_back(cube);
    primitives.push_back(plane);
    //primitives.push_back(sphere);
}

void Game::Update(float dt)
{
    collision.update(primitives);
}

void Game::Render()
{
    //shader.Use();
    //glm::mat4 projection = glm::perspective(glm::radians(myCamera->Zoom), (float)Width / (float)Height, 0.1f, 100.0f);
    //shader.SetMatrix4("projection", projection);
    //glm::mat4 view = myCamera->GetViewMatrix();
    //shader.SetMatrix4("view", view);
    //Draw primitives
    for (int i = 0; i < primitives.size(); i++) {
        //glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // Change the offsets as needed
        ////if the primitives is plane (with get info) place at 0,0,0
        //if (primitives[i]->getInfo() == "Plane") {
        //    position = glm::vec3(0.0f, 0.0f, 0.0f);
        //} else if (primitives[i]->getInfo() == "Sphere") {
        //    position = glm::vec3(0.5f * i, 0.0f, 0.5f * i);
        //} else if (primitives[i]->getInfo() == "Cube") {
        //    position = glm::vec3(-0.5f * i, 0.0f, -0.5f * i);
        //}
        primitives[i]->draw(shader, primitives[i]->position, *myCamera);
        if (primitives[i]->collisionEnabled) {
            primitives[i]->drawHitbox(hitboxShader, primitives[i]->position, *myCamera);
        }
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
        //activated desactivated depth test and face culing with u and i
        if (this->Keys[GLFW_KEY_M]){
            glEnable(GL_DEPTH_TEST);
            std::cout << "Depth test is enabled" << std::endl;
        }
        if (this->Keys[GLFW_KEY_N]){
            glDisable(GL_DEPTH_TEST);
            std::cout << "Depth test is disabled" << std::endl;
        }
        //v b for face culling
        if (this->Keys[GLFW_KEY_V]){
            glEnable(GL_CULL_FACE);
            std::cout << "Face culling is enabled" << std::endl;
        }
        if (this->Keys[GLFW_KEY_B]){
            glDisable(GL_CULL_FACE);
            std::cout << "Face culling is disabled" << std::endl;
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
                } else if (primitives[i]->getInfo() == "Sphere") {
                    primitives[i]->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
                } else if (primitives[i]->getInfo() == "Cube") {
                    primitives[i]->setPosition(glm::vec3(0.0f, 3.0f, 0.0f));
                }
            }
            std::cout << "All primitives are back to their original position" << std::endl;
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
