#pragma once
#include <vector>
#include <glm/glm.hpp>  // For vec3 and vec4 types
#include "../shaders/shader.h"
#include "../camera/camera.h"
#include "shadows.h"

class Light {
public:
    enum class LightType {
        AMBIENT,
        POINT,
        DIRECTIONAL,
        SPOTLIGHT
    };

    struct LightData {
        LightType type;
        glm::vec4 color;  // RGBA color (use vec3 for RGB if alpha is not needed)
        glm::vec3 position;  // Position (relevant for point and spotlight)
        glm::vec3 direction; // Direction (relevant for directional and spotlight)
        float intensity;     // Intensity for lighting effect (range [0,1])
        
        // Spotlight-specific properties
        float cutOff;
        float outerCutOff;
        //lightSpaceMatrix
        glm::mat4 lightSpaceMatrix;
        unsigned int depthMapFBO;
        unsigned int depthMap;
    };

    Light();
    ~Light();

    // Methods to add various types of lights
    void addAmbientLight(const glm::vec4& color, float intensity);
    void addPointLight(const glm::vec3& position, const glm::vec4& color, float intensity);
    void addDirectionalLight(const glm::vec3& position, const glm::vec3& target, const glm::vec4& color, float intensity);
    void addSpotlight(const glm::vec3& position, const glm::vec3& direction, const glm::vec4& color, float intensity, float cutOff, float outerCutOff);

    void printLightInfo() const;  // Method for testing and debugging

    void useLight(Shader& shader, Camera& camera);

    void renderDepthBuffer(Shader& shader, Camera& camera);

    //set the type of light
    void setType(LightType type);
    //set the color of the light
    void setColor(glm::vec4 color);
    //set the intensity of the light
    void setIntensity(float intensity);
    //set the position of the light take position and int i
    void setPosition(glm::vec3 position, int i);
    //set the direction of the light
    void setDirection(glm::vec3 direction);
    //set the cut off of the light
    void setCutOff(float cutOff);
    //set the outer cut off of the light
    void setOuterCutOff(float outerCutOff);
    //get lights vector and take the int i
    LightData* getLight(int i);
    //get position of the light i
    glm::vec3 getPosition(int i);
    //get the light vector
    std::vector<LightData*> getLights();

    void useOneLight(Shader& shader, Camera& camera, int i);
    void useOneLightPoint(Shader& shader, Camera& camera, int i);
    std::vector<glm::mat4> getLightSpaceMatricesFromPointLight(int i);

    unsigned int create_depth_map(unsigned int width, unsigned int height, unsigned int& depthMapFBO);
    unsigned int create_depth_map_point(unsigned int width, unsigned int height, unsigned int& depthMapFBO);

    float shadowWidth;
    float shadowHeight;

    float near_plane = 1.0f;
    float far_plane = 25.0f;

    void setShadowWidth(float width);
    void setShadowHeight(float height);

    void setNearPlane(float near);
    void setFarPlane(float far);

private:
    std::vector<LightData*> lights;  // Vector of pointers to LightData

    // Helper method for creating a new light and adding it to the vector
    LightData* createLight(LightType type, const glm::vec4& color, float intensity);

    glm::mat4 lightProjectionViewDirect(glm::vec3 lightPos, glm::vec3 lightDir, float near_plane, float far_plane, float width, float height);
    glm::mat4 lightProjectionViewSpot(glm::vec3 lightPos, glm::vec3 lightDir, float cutOff, float outerCutOff, float near_plane, float far_plane);
};