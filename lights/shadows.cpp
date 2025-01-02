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

//void Shadows::renderDepthBuffer(Shader& shader, Camera& camera, std::vector<Light*> lights, std::vector<Primitives*> primitives)
//{
//    shader.Use();
//    //lightCount
//    shader.SetInteger("lightCount", static_cast<int>(lights.size()));
//    for (unsigned int i = 0; i < lights.size(); i++) {
//        //check if the light is a directional light or a spotlight
//        if (lights[i]->type == Light::LightType::DIRECTIONAL) {
//            //add to vector
//            lightSpaceMatrices.push_back(lightProjectionViewDirect(lights[i]->position, lights[i]->direction, camera.GetFarPlane(), camera.GetFarPlane()));
//            shader.SetMatrix4(("lightSpaceMatrix[" + std::to_string(i) + "]").c_str(), lightSpaceMatrices[i]);
//
//            //loop over the primitives
//            for (unsigned int j = 0; j < primitives.size(); j++) {
//                glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//                glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//                glClear(GL_DEPTH_BUFFER_BIT);
//                primitives[j]->draw(shader, camera);
//                glBindBuffer(GL_FRAMEBUFFER, 0);
//            }
//        } if (lights[i]->type == Light::LightType::SPOTLIGHT) {
//            //add to vector
//            lightSpaceMatrices.push_back(lightProjectionViewSpot(lights[i]->position, lights[i]->direction, lights[i]->cutOff, lights[i]->outerCutOff, camera.GetFarPlane(), camera.GetFarPlane()));
//            shader.SetMatrix4(("lightSpaceMatrix[" + std::to_string(i) + "]").c_str(), lightSpaceMatrices[i]);
//            //loop over the primitives
//            for (unsigned int j = 0; j < primitives.size(); j++) {
//                glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//                glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//                glClear(GL_DEPTH_BUFFER_BIT);
//                primitives[j]->draw(shader, camera);
//                glBindBuffer(GL_FRAMEBUFFER, 0);
//            }
//        } if (lights[i]->type == Light::LightType::POINT) {
//            //lightSpaceMatrices.push_back(lightProjectionViewPoint(lights[i]->position));
//        }
//    }
//}

//void Shadows::renderShader(Shader& shader, Camera& camera, std::vector<Light::LightData*> lights, std::vector<Primitives*> primitives){
//    shader.Use();
//    //loop over the lights
//    for (unsigned int i = 0; i < lights.size(); i++) {
//        //check if the light is a directional light or a spotlight
//        if (lights[i]->type == Light::LightType::DIRECTIONAL) {
//            //add to vector
//            lightSpaceMatrices.push_back(lightProjectionViewDirect(lights[i]->position, lights[i]->direction, camera.GetFarPlane(), camera.GetFarPlane()));
//            shader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrices[i]);
//
//            //loop over the primitives
//            for (unsigned int j = 0; j < primitives.size(); j++) {
//                glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//                glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//                glClear(GL_DEPTH_BUFFER_BIT);
//                primitives[j]->draw(shader, camera);
//                glBindBuffer(GL_FRAMEBUFFER, 0);
//            }
//        } if (lights[i]->type == Light::LightType::SPOTLIGHT) {
//            //add to vector
//            lightSpaceMatrices.push_back(lightProjectionViewSpot(lights[i]->position, lights[i]->direction, lights[i]->cutOff, lights[i]->outerCutOff, camera.GetFarPlane(), camera.GetFarPlane()));
//            shader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrices[i]);
//        } if (lights[i]->type == Light::LightType::POINT) {
//            //lightSpaceMatrices.push_back(lightProjectionViewPoint(lights[i]->position));
//        }
//    }
//}

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

glm::mat4 Shadows::lightProjectionViewDirect(glm::vec3 lightPos, glm::vec3 lightDir, float near_plane, float far_plane)
{
    // Calculate the light's projection and view matrices
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
    return lightSpaceMatrix;
}

glm::mat4 Shadows::lightProjectionViewSpot(glm::vec3 lightPos, glm::vec3 lightDir, float cutOff, float outerCutOff, float near_plane, float far_plane)
{
    // Calculate the light's projection and view matrices
    lightProjection = glm::perspective(glm::radians(outerCutOff * 2), 1.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
    return lightSpaceMatrix;
}



unsigned int Shadows::getDepthMap()
{
    return depthMap;
}