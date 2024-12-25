#pragma once

#include "../glad/glad.h"
#include <iostream>
#include "../camera/camera.h"
#include "../shaders/shader.h"
#include <vector>
#include <random>

class ssaoBuffer
{

public:
    ssaoBuffer(int width, int height);
    ~ssaoBuffer();

    void BindFramebuffer();
    void UnbindFramebuffer();

    void RenderWithSSAO(Shader& shader, Camera& camera);
    void RenderWithSSAOBlur(Shader& shader, Camera& camera);

    void Update(int width, int height);
    void renderQuad();

    std::vector<glm::vec3> getSSAOKernel();
    std::vector<glm::vec3> getSSAONoise();
    void bindUniformsSSAO(Shader& shader);
    void bindUniformsSSAOBlur(Shader& shader);
    void bindFBO();
    void bindBlurFBO();
    unsigned int getSSAOTexture();

private:

    GLuint positionTexture;
    GLuint normalTexture;
    GLuint noiseTexture;

    GLuint framebuffer;
    GLuint ssaoFBO, ssaoBlurFBO;
    GLuint ssaoColorBuffer, ssaoColorBufferBlur;

    GLuint quadVAO, quadVBO;

    std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;

    void InitFramebuffer(int width, int height);
    void InitQuad();
    float ourLerp(float a, float b, float f);


};
