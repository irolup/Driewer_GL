#include "camera.h"
#include <GLFW/glfw3.h> // Include GLFW header for glfwGetKey

// Constructor with default values
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, int width, int height)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
      MovementSpeed(SPEED), 
      MouseSensitivity(SENSITIVITY), 
      Zoom(ZOOM), 
      Width(width), 
      Height(height),
      isJumping(false),         // Initialize jump state to false
      initialY(position.y),     // Set the initial Y position to the camera's starting Y
      JumpVelocity(0.0f),       // Start with no vertical velocity
      Gravity(-9.8f)            // Use gravity to bring the camera down (adjust as needed)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, int width, int height)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
      MovementSpeed(SPEED), 
      MouseSensitivity(SENSITIVITY), 
      Zoom(ZOOM), 
      Width(width), 
      Height(height)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Constructor with width and height
Camera::Camera(int width, int height, glm::vec3 position)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
      MovementSpeed(SPEED), 
      MouseSensitivity(SENSITIVITY), 
      Zoom(ZOOM), 
      Width(width), 
      Height(height),
      isJumping(false),         // Initialize jump state to false
      initialY(position.y),     // Set the initial Y position to the camera's starting Y
      JumpVelocity(0.0f),       // Start with no vertical velocity
      Gravity(-9.8f)            // Use gravity to bring the camera down (adjust as needed)
{
    Position = position;
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    Yaw = YAW;
    Pitch = PITCH;
    updateCameraVectors();
}

// Returns the view matrix
glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

// Returns the projection matrix with custom near and far planes
glm::mat4 Camera::GetProjectionMatrix(float nearPlane, float farPlane)
{
    float aspectRatio = static_cast<float>(Width) / static_cast<float>(Height);
    return glm::perspective(glm::radians(Zoom), aspectRatio, nearPlane, farPlane);
}

// Returns the projection matrix with default near and far planes
glm::mat4 Camera::GetProjectionMatrix()
{
    return GetProjectionMatrix(DEFAULT_NEAR_PLANE, DEFAULT_FAR_PLANE);
}

// Processes keyboard input
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

// Processes mouse movement
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
    }

    updateCameraVectors();
}

// Processes mouse scroll
void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f) Zoom = 1.0f;
    if (Zoom > 45.0f) Zoom = 45.0f;
}

// Updates camera vectors based on Euler angles
void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::ProcessMovement(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;

    // Calculate the movement direction, but constrain to the XZ-plane
    glm::vec3 forwardXZ = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));

    if (direction == FORWARD)
        Position += forwardXZ * velocity;  // Move forward in XZ plane
    if (direction == BACKWARD)
        Position -= forwardXZ * velocity;  // Move backward in XZ plane
    if (direction == LEFT)
        Position -= Right * velocity;      // Move left (strafe)
    if (direction == RIGHT)
        Position += Right * velocity;      // Move right (strafe)
}

void Camera::ProcessJump(float deltaTime, GLFWwindow* window)
{
    // Check for jump input (spacebar) and ensure the camera is not already jumping
    if (!isJumping && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        isJumping = true;
        JumpVelocity = 5.0f;  // Initial upward velocity for jump (tweak to control height)
    }

    // If the camera is jumping or in the air, apply gravity
    if (isJumping || Position.y > initialY)
    {
        // Apply gravity to the jump velocity
        JumpVelocity += Gravity * deltaTime;

        // Update the camera's Y position with the jump velocity
        Position.y += JumpVelocity * deltaTime;

        // If the camera has returned to or below its initial Y position, stop the jump
        if (Position.y <= initialY)
        {
            Position.y = initialY;  // Reset the Y position to the initial value
            isJumping = false;      // Stop jumping
            JumpVelocity = 0.0f;    // Reset the jump velocity
        }
    }
}

void Camera::SetWindowSize(int width, int height)
{
    Width = width;
    Height = height;
}