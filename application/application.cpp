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

    myCamera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f));

    //Cube object
    cube = new Cube();
    plane = new Plane();
    sphere = new Sphere();

    primitives.push_back(cube);
    primitives.push_back(plane);
    primitives.push_back(sphere);
}

void Game::Update(float dt)
{
    
}

void Game::Render()
{
    shader.Use();
    glm::mat4 projection = glm::perspective(glm::radians(myCamera->Zoom), (float)Width / (float)Height, 0.1f, 100.0f);
    shader.SetMatrix4("projection", projection);
    glm::mat4 view = myCamera->GetViewMatrix();
    shader.SetMatrix4("view", view);
    //Draw primitives
    for (int i = 0; i < primitives.size(); i++) {
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // Change the offsets as needed
        //if the primitives is plane (with get info) place at 0,0,0
        if (primitives[i]->getInfo() == "Plane") {
            position = glm::vec3(0.0f, 0.0f, 0.0f);
        } else if (primitives[i]->getInfo() == "Sphere") {
            position = glm::vec3(0.5f * i, 0.0f, 0.5f * i);
        } else if (primitives[i]->getInfo() == "Cube") {
            position = glm::vec3(-0.5f * i, 0.0f, -0.5f * i);
        }
        primitives[i]->draw(shader, position, myCamera->Position);
    }
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_MENU)
    {
        float cameraSpeed = 5.f * dt;
        if (this->Keys[GLFW_KEY_W] ){
            myCamera->ProcessKeyboard(Camera_Movement::FORWARD, cameraSpeed);
            //log to test the movement
            std::cout << "Camera position: " << myCamera->Position.x << " " << myCamera->Position.y << " " << myCamera->Position.z << std::endl;
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

//collision detection
void Game::DoCollisions()
{
    //for (Primitives* primitive : primitives) {
    //    if (primitive->getInfo() == "Cube") {
    //        if (cube->checkCollision(*primitive)) {
    //            std::cout << "Cube collided with cube" << std::endl;
    //        }
    //    }
    //    if (primitive->getInfo() == "Sphere") {
    //        if (sphere->checkCollision(*primitive)) {
    //            std::cout << "Sphere collided with sphere" << std::endl;
    //        }
    //    }
    //    if (primitive->getInfo() == "Plane") {
    //        if (plane->checkCollision(*primitive)) {
    //            std::cout << "Plane collided with plane" << std::endl;
    //        }
    //    }
    //}
}

