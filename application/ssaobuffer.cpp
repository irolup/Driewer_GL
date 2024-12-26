#include "ssaobuffer.h"

ssaoBuffer::ssaoBuffer(int width, int height, Shader& ssao, Shader& ssaoBlur)
{
    InitFramebuffer(width, height);
    InitQuad();
    ssaoKernel = getSSAOKernel();
    ssaoNoise = getSSAONoise();
    //set uniforms
    ssao.Use();
    ssao.SetInteger("gPosition", 0);
    ssao.SetInteger("gNormal", 1);
    ssao.SetInteger("texNoise", 2);

    ssaoBlur.Use();
    ssaoBlur.SetInteger("ssaoInput", 0);

}

ssaoBuffer::~ssaoBuffer(){

    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &positionTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &noiseTexture);
    glDeleteFramebuffers(1, &ssaoFBO);
    glDeleteFramebuffers(1, &ssaoBlurFBO);
    glDeleteTextures(1, &ssaoColorBuffer);
    glDeleteTextures(1, &ssaoColorBufferBlur);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}

void ssaoBuffer::InitFramebuffer(int width, int height)
{

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

    // Noise texture
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, noiseTexture, 0);

    // Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    //unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void ssaoBuffer::InitQuad()
{
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

void ssaoBuffer::BindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void ssaoBuffer::UnbindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void ssaoBuffer::Update(int width, int height)
{
    glDeleteTextures(1, &positionTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &noiseTexture);

    InitFramebuffer(width, height);
}

void ssaoBuffer::renderQuad()
{
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

std::vector<glm::vec3> ssaoBuffer::getSSAOKernel() {
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = ourLerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
    return ssaoKernel;
}

float ssaoBuffer::ourLerp(float a, float b, float f)
{
    return a + f * (b - a);
}

std::vector<glm::vec3> ssaoBuffer::getSSAONoise(){
    std::vector<glm::vec3> ssaoNoise;
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    return ssaoNoise;
}

void ssaoBuffer::bindUniformsSSAO(Shader& shader){
    shader.Use();
    shader.SetInteger("gPosition", 0);
    shader.SetInteger("gNormal", 1);
    shader.SetInteger("texNoise", 2);
}

void ssaoBuffer::bindUniformsSSAOBlur(Shader& shader){
    shader.Use();
    shader.SetInteger("ssaoInput", 0);
}

void ssaoBuffer::bindFBO(){
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ssaoBuffer::bindBlurFBO(){
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ssaoBuffer::RenderWithSSAO(Shader& shader, Camera& camera){
    bindFBO();

    
    for (unsigned int i = 0; i < 64; ++i)
    {
        shader.SetVector3f(("samples[" + std::to_string(i) + "]").c_str(), ssaoKernel[i]);
    }

    shader.SetMatrix4("projection", camera.GetProjectionMatrix());
    

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, positionTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);

    renderQuad();
    UnbindFramebuffer();
}

void ssaoBuffer::RenderWithSSAOBlur(Shader& shader, Camera& camera){
    bindBlurFBO();


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);

    renderQuad();
    UnbindFramebuffer();
}

unsigned int ssaoBuffer::getSSAOTexture(){
    return ssaoColorBuffer;
}