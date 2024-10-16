#ifndef CAMERA_H
#define CAMERA_H

#include "../glad/glad.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Defines several possible options for camera movement
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

// Default camera resolution values
const int DEFAULT_WIDTH  = 920;
const int DEFAULT_HEIGHT = 800;

// Default near and far clipping planes
const float DEFAULT_NEAR_PLANE = 0.1f;
const float DEFAULT_FAR_PLANE  = 100.0f;

// Camera class
class Camera {
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Camera resolution with width and height
    int Width;
    int Height;

    // Euler Angles
    float Yaw;
    float Pitch;

    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Jumping-related variables
    bool isJumping;           // Flag to check if the camera is jumping
    float initialY;           // Store the initial Y position
    float JumpVelocity;       // Controls jump strength
    float Gravity;            // Simulates gravity


    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
           float yaw = YAW, 
           float pitch = PITCH,
           int width = DEFAULT_WIDTH, 
           int height = DEFAULT_HEIGHT);

    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, 
           float upX, float upY, float upZ, 
           float yaw, float pitch,
           int width = DEFAULT_WIDTH, 
           int height = DEFAULT_HEIGHT);

    //Constructor with width and height and position
    Camera(int width, int height, glm::vec3 position);

    // Returns the view matrix
    glm::mat4 GetViewMatrix();

    // Returns the projection matrix with custom near and far planes
    glm::mat4 GetProjectionMatrix(float nearPlane, float farPlane);

    // Returns the projection matrix with default near and far planes
    glm::mat4 GetProjectionMatrix();

    // Processes keyboard input
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // Processes mouse movement
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // Processes mouse scroll
    void ProcessMouseScroll(float yoffset);

    void ProcessMovement(Camera_Movement direction, float deltaTime);

    //need to take window as parameter for checking spacebar
    void ProcessJump(float deltaTime, GLFWwindow* window);

    //func to set the window size
    void SetWindowSize(int width, int height);

    //Get camera pitch angle
    float getPitch();

    //Get camera yaw angle
    float getYaw();

private:
    // Updates camera vectors based on Euler angles
    void updateCameraVectors();
};

#endif // CAMERA_H