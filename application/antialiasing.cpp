#include "antialiasing.h"

Antialiasing::Antialiasing(int width, int height) {
    InitFramebuffer(width, height);
    InitQuad();
}

Antialiasing::~Antialiasing() {
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &colorTexture);
    glDeleteTextures(1, &historyTexture);
    glDeleteTextures(1, &depthTexture); // Delete the depth texture
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}

void Antialiasing::InitFramebuffer(int width, int height) {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create color texture
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    // Create history texture
    glGenTextures(1, &historyTexture);
    glBindTexture(GL_TEXTURE_2D, historyTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, historyTexture, 0);

    // Create depth texture
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);


    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Antialiasing::InitQuad() {
    float quadVertices[] = {
        // positions     // texCoords     // motion vectors and depth (if needed)
        -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Position, TexCoord, MotionVector, Depth
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); // TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); // Motion Vectors
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float))); // Depth
}

void Antialiasing::BindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for 3D rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Antialiasing::RenderWithShader(Shader& shader, Camera& camera) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Bind default framebuffer for final output
    glDisable(GL_DEPTH_TEST);              // Disable depth test for 2D quad render
    shader.Use();
    shader.SetInteger("screenTexture", 0);
    shader.SetInteger("historyTexture", 1);
    shader.SetInteger("depthTexture", 2);  // Set the depth texture uniform
    shader.SetVector2f("inverseScreenSize", 1.0f / camera.Width, 1.0f / camera.Height);
    float baseBlendFactor = 0.2f; // Example value; adjust as needed
    float depthThreshold = 0.02f;  // Example threshold for depth comparison
    shader.SetFloat("baseBlendFactor", baseBlendFactor);
    shader.SetFloat("depthThreshold", depthThreshold);

    // Bind the current frame texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);

    // Bind the history texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, historyTexture);

    // Bind the depth texture
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glEnable(GL_DEPTH_TEST); // Re-enable for next render pass if needed
}

GLuint Antialiasing::GetTexture() const {
    return colorTexture;
}

void Antialiasing::UpdateHistoryBuffer(Camera& camera) {
    // Update the history texture to the current color texture
    glBindTexture(GL_TEXTURE_2D, historyTexture);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, camera.Width, camera.Height, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Antialiasing::Resize(int width, int height) {
    // Delete existing textures
    glDeleteTextures(1, &colorTexture);
    glDeleteTextures(1, &historyTexture);
    glDeleteTextures(1, &depthTexture);

    // Reinitialize the framebuffer with the new size
    InitFramebuffer(width, height);
}

void Antialiasing::Update(int width, int height) {
    Resize(width, height);
}