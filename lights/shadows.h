#ifndef SHADOWS_H

#define SHADOWS_H
//iunclude glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "lights.h"
#include "../primitives/primitives.h"
#include <iostream>

class Light;

class Shadows
{
    public:

        Shadows();
        ~Shadows();

        void init(unsigned int width, unsigned int height);
        //void renderDepthBuffer(Shader& shader, Camera& camera, std::vector<Light::LightData*> lights, std::vector<Primitives*> primitives);
        //void renderShader(Shader& shader, Camera& camera, std::vector<Light::LightData*> lights, std::vector<Primitives*> primitives);
        void update();
        void setNearFar(float near, float far);
        //get depth map
        unsigned int getDepthMap();

        //section for shadow mapping (directional light and spot light)
        unsigned int depthMapFBO;
        unsigned int SHADOW_WIDTH, SHADOW_HEIGHT;
        //depth map texture
        unsigned int depthMap;
        //border color
        float borderColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};

        
        glm::mat4 lightProjectionViewDirect(glm::vec3 lightPos, glm::vec3 lightDir, float near_plane, float far_plane);
        glm::mat4 lightProjectionViewSpot(glm::vec3 lightPos, glm::vec3 lightDir, float cutOff, float outerCutOff, float near_plane, float far_plane);

    private:
    //float aspect = (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT;
    //float near = 1.0f;
    //float far = 25.0f;
    //glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far); 
    //set uniform for shadow mapping
    void setUniforms(Shader& shader, Camera& camera);

    
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane, far_plane;
    //vector of lightSpaceMatrix
    std::vector<glm::mat4> lightSpaceMatrices;

};

#endif // SHADOW_H