#pragma once

#include "../glad/glad.h"
#include <iostream>
#include "../camera/camera.h"
#include "../shaders/shader.h"

class GBuffer {
public:
    enum class Type {
        BASIC,
        ADVANCED
    };

    GBuffer(int width, int height, Type type = Type::BASIC);
    ~GBuffer();

    void BindFramebuffer();
    void RenderWithShader(Shader& shader, Camera& camera);
    GLuint GetTexture(GLuint attachmentIndex) const;
    void Update(int width, int height);

private:
    GLuint framebuffer;
    GLuint positionTexture;
    GLuint normalTexture;
    GLuint albedoTexture;
    GLuint depthTexture;
    GLuint quadVAO, quadVBO;

    Type bufferType;

    void InitFramebuffer(int width, int height);
    void InitQuad();
    void Resize(int width, int height);
};