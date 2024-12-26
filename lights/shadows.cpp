#include "shadows.h"

Shadows::Shadows()
{
    //ctor
}

Shadows::~Shadows()
{
    //dtor
}

void Shadows::init(unsigned int width, unsigned int height)
{
    // Initialize the shadow map
    SHADOW_WIDTH = width;
    SHADOW_HEIGHT = height;

    // Create the depth map FBO
    glGenFramebuffers(1, &depthMapFBO);
    // Create the depth map texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // Attach the depth map texture to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



}

void Shadows::renderDepthBuffer(Shader& shader, Camera& camera, std::vector<Light::LightData*> lights)
{
    // Render the shadow map
    shader.Use();
    //set int for shadow mapping
    calculateLightSpaceMatrix(lights);
    
    int numberOflightSpaceMatrices = lightSpaceMatrices.size();
    shader.SetInteger("numberOflightSpaceMatrices", numberOflightSpaceMatrices);

    for (unsigned int i = 0; i < lightSpaceMatrices.size(); i++) {
        //set the lightSpaceMatrix // Send the array of matrices
        shader.SetMatrix4(("lightSpaceMatrices[" + std::to_string(i) + "]").c_str(), lightSpaceMatrices[i]);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

    }
}

void Shadows::renderShader(Shader& shader, Camera& camera, std::vector<Light::LightData*> lights){
    shader.Use();
    calculateLightSpaceMatrix(lights);
    int numberOflightSpaceMatrices = lightSpaceMatrices.size();
    shader.SetInteger("numberOflightSpaceMatrices", numberOflightSpaceMatrices);
    for (unsigned int i = 0; i < lightSpaceMatrices.size(); i++) {
        //set the lightSpaceMatrix // Send the array of matrices
        shader.SetMatrix4(("lightSpaceMatrices[" + std::to_string(i) + "]").c_str(), lightSpaceMatrices[i]);
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, depthMap);
    }

}

void Shadows::update()
{
    // Update the shadow map
}

void Shadows::setUniforms(Shader& shader, Camera& camera)
{
    shader.Use();
    //set int for shadow mapping
    //shader.SetInteger("shadowMap", 0);
}

void Shadows::setNearFar(float near, float far)
{
    near_plane = near;
    far_plane = far;
}

glm::mat4 Shadows::lightProjectionView(glm::vec3 lightPos, glm::vec3 lightDir)
{
    // Calculate the light's projection and view matrices
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
    return lightSpaceMatrix;
}

void Shadows::calculateLightSpaceMatrix(std::vector<Light::LightData*> lights)
{
    // Calculate the light space matrix
    for (unsigned int i = 0; i < lights.size(); i++) {
        //check if the light is a directional light or a spotlight
        if (lights[i]->type == Light::LightType::DIRECTIONAL || lights[i]->type == Light::LightType::SPOTLIGHT) {
            lightSpaceMatrices.push_back(lightProjectionView(lights[i]->position, lights[i]->direction));
        }
    }
}

unsigned int Shadows::getDepthMap()
{
    return depthMap;
}