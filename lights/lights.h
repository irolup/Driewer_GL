#pragma once
#include <vector>
#include <glm/glm.hpp>  // For vec3 and vec4 types
#include "../shaders/shader.h"
#include "../camera/camera.h"

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
    };

    Light();
    ~Light();

    // Methods to add various types of lights
    void addAmbientLight(const glm::vec4& color, float intensity);
    void addPointLight(const glm::vec3& position, const glm::vec4& color, float intensity);
    void addDirectionalLight(const glm::vec3& direction, const glm::vec4& color, float intensity);
    void addSpotlight(const glm::vec3& position, const glm::vec3& direction, const glm::vec4& color, float intensity, float cutOff, float outerCutOff);

    void printLightInfo() const;  // Method for testing and debugging

    void useLight(Shader& shader, Camera& camera);

    //set the type of light
    void setType(LightType type);
    //set the color of the light
    void setColor(glm::vec4 color);
    //set the intensity of the light
    void setIntensity(float intensity);
    //set the position of the light
    void setPosition(glm::vec3 position);
    //set the direction of the light
    void setDirection(glm::vec3 direction);
    //set the cut off of the light
    void setCutOff(float cutOff);
    //set the outer cut off of the light
    void setOuterCutOff(float outerCutOff);

private:
    std::vector<LightData*> lights;  // Vector of pointers to LightData

    // Helper method for creating a new light and adding it to the vector
    LightData* createLight(LightType type, const glm::vec4& color, float intensity);
};