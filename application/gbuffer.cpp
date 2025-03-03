#include "gbuffer.h"

GBuffer::GBuffer(int width, int height, Type type) : bufferType(type) {
    InitFramebuffer(width, height);
    InitQuad();
}

GBuffer::~GBuffer() {
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &positionTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &albedoMetallicTexture);
    glDeleteTextures(1, &specularRoughnessTexture);
    glDeleteTextures(1, &fresnelOcclusionTexture);
    glDeleteTextures(1, &ambiantBrightnessTexture);
    glDeleteTextures(1, &depthTexture);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}

void GBuffer::InitFramebuffer(int width, int height) {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Position texture
    glGenTextures(1, &positionTexture);
    glBindTexture(GL_TEXTURE_2D, positionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionTexture, 0);

    // Normal texture
    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);

    // Albedo + Metallic texture
    glGenTextures(1, &albedoMetallicTexture);
    glBindTexture(GL_TEXTURE_2D, albedoMetallicTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedoMetallicTexture, 0);

    // Specular + Roughness texture
    glGenTextures(1, &specularRoughnessTexture);
    glBindTexture(GL_TEXTURE_2D, specularRoughnessTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, specularRoughnessTexture, 0);

    // Fresnel + Occlusion texture
    glGenTextures(1, &fresnelOcclusionTexture);
    glBindTexture(GL_TEXTURE_2D, fresnelOcclusionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, fresnelOcclusionTexture, 0);

    // Brightness texture
    glGenTextures(1, &ambiantBrightnessTexture);
    glBindTexture(GL_TEXTURE_2D, ambiantBrightnessTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, ambiantBrightnessTexture, 0);

    // Depth texture
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    GLuint attachments[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
                              GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
    glDrawBuffers(6, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void GBuffer::InitQuad() {
    float quadVertices[] = {
        // positions       // texCoords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // TexCoords
}

void GBuffer::BindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GBuffer::UnbindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::RenderWithShader(Shader& shader, Camera& camera, float ao) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glDisable(GL_DEPTH_TEST);
    shader.Use();

    shader.SetInteger("gPosition", 0);
    shader.SetInteger("gNormal", 1);
    shader.SetInteger("gAlbedoMetallic", 2);
    shader.SetInteger("gSpecularRoughness", 3);
    shader.SetInteger("gFresnelOcclusion", 4);
    shader.SetInteger("gAmbiantBrightness", 5);
    shader.SetInteger("gDepth", 6);
    shader.SetFloat("ao_slider", ao);

    

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, positionTexture);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, normalTexture);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, albedoMetallicTexture);
    glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, specularRoughnessTexture);
    glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, fresnelOcclusionTexture);
    glActiveTexture(GL_TEXTURE5); glBindTexture(GL_TEXTURE_2D, ambiantBrightnessTexture);
    glActiveTexture(GL_TEXTURE6); glBindTexture(GL_TEXTURE_2D, depthTexture);

    shader.SetVector3f("viewPos", camera.Position);

    //view pos
    

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    //unbind textures
    for (unsigned int i = 0; i < 7; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    //glEnable(GL_DEPTH_TEST);
}


void GBuffer::RenderWithShaderSSAO(Shader& shader, Camera& camera, unsigned int ssaoTexture) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glDisable(GL_DEPTH_TEST);
    shader.Use();

    shader.SetInteger("gPosition", 0);
    shader.SetInteger("gNormal", 1);
    shader.SetInteger("gAlbedoMetallic", 2);
    shader.SetInteger("gSpecularRoughness", 3);
    shader.SetInteger("gFresnelOcclusion", 4);
    shader.SetInteger("gAmbiantBrightness", 5);
    shader.SetInteger("gDepth", 6);
    shader.SetInteger("gSSAO", 7);
    

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, positionTexture);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, normalTexture);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, albedoMetallicTexture);
    glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, specularRoughnessTexture);
    glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, fresnelOcclusionTexture);
    glActiveTexture(GL_TEXTURE5); glBindTexture(GL_TEXTURE_2D, ambiantBrightnessTexture);
    glActiveTexture(GL_TEXTURE6); glBindTexture(GL_TEXTURE_2D, depthTexture);
    glActiveTexture(GL_TEXTURE7); glBindTexture(GL_TEXTURE_2D, ssaoTexture);

    shader.SetVector3f("viewPos", camera.Position);

    //view pos
    

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    //unbind textures
    for (unsigned int i = 0; i < 8; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    //glEnable(GL_DEPTH_TEST);
}

GLuint GBuffer::GetTexture(GLuint attachmentIndex) const {
    switch (attachmentIndex) {
        case 0: return positionTexture;
        case 1: return normalTexture;
        case 2: return albedoMetallicTexture;
        case 3: return specularRoughnessTexture;
        case 4: return fresnelOcclusionTexture;
        case 5: return ambiantBrightnessTexture;
        case 6: return depthTexture;
        default: return 0;
    }
}

void GBuffer::Resize(int width, int height) {
    glDeleteTextures(1, &positionTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &albedoMetallicTexture);
    glDeleteTextures(1, &specularRoughnessTexture);
    glDeleteTextures(1, &fresnelOcclusionTexture);
    glDeleteTextures(1, &ambiantBrightnessTexture);
    glDeleteTextures(1, &depthTexture);

    InitFramebuffer(width, height);
}

void GBuffer::Update(int width, int height) {
    Resize(width, height);
}

void GBuffer::renderQuad() {
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}