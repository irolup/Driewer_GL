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
    void UnbindFramebuffer();
    void RenderWithShader(Shader& shader, Camera& camera, float ao);
    void RenderWithShaderSSAO(Shader& shader, Camera& camera, unsigned int ssaoTexture);
    GLuint GetTexture(GLuint attachmentIndex) const;
    void Update(int width, int height);
    void renderQuad();
    GLuint framebuffer;

private:
    
    GLuint positionTexture;
    GLuint normalTexture;
    GLuint albedoMetallicTexture;
    GLuint specularRoughnessTexture;
    GLuint fresnelOcclusionTexture;
    GLuint ambiantBrightnessTexture;
    GLuint depthTexture;
    GLuint quadVAO, quadVBO;

    Type bufferType;

    void InitFramebuffer(int width, int height);
    void InitQuad();
    void Resize(int width, int height);
    
};