// Antialiasing.h
#ifndef ANTIALIASING_H
#define ANTIALIASING_H

#include "../glad/glad.h"
#include "../shaders/shader.h"
#include "../camera/camera.h"
#include <iostream>

class Antialiasing {
public:
    enum class Type {
            NONE,
            MSAA,
            SMAA
        };
    Antialiasing(int width, int height, Type type = Type::NONE);
    ~Antialiasing();

    void BindFramebuffer();                     // Bind framebuffer for main rendering pass
    void RenderWithShader(Shader& shader, Camera& camera);  // Render the scene with the selected antialiasing shader
    GLuint GetTexture() const;                  // Get the texture attached to the framebuffer
    void UpdateHistoryBuffer(Camera& camera);   // Update the history buffer with the current color texture
    //update if resized
    void Update(int width, int height);

private:
    GLuint framebuffer;
    GLuint colorTexture;
    GLuint historyTexture;
    GLuint depthTexture;
    GLuint quadVAO, quadVBO;

    // Antialiasing type
    Type aaType;

    
    void InitFramebuffer(int width, int height);
    void InitMSAAFramebuffer(int width, int height);  // MSAA initialization
    void InitQuad();
    //resize function
    void Resize(int width, int height);
};

#endif // ANTIALIASING_H