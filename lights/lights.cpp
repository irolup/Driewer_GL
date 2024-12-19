#include "lights.h"
#include <iostream>

// Constructor
Light::Light() {}

// Destructor to free light memory
Light::~Light() {
    for (LightData* light : lights) {
        delete light;
    }
    lights.clear();
}

// Helper function to create a light and add it to the vector
Light::LightData* Light::createLight(LightType type, const glm::vec4& color, float intensity) {
    LightData* newLight = new LightData;
    newLight->type = type;
    newLight->color = color;
    newLight->intensity = intensity;

    // Initialize other properties
    newLight->position = glm::vec3(0.0f); // Default position, or pass as parameter
    newLight->direction = glm::vec3(0.0f); // Default direction, or pass as parameter
    newLight->cutOff = 0.0f; // Default cutoff, can be changed later for spotlights
    newLight->outerCutOff = 0.0f; // Default outer cutoff

    lights.push_back(newLight);
    std::cout << "Light added" << std::endl;
    std::cout << "Number of lights in lights: " << lights.size() << std::endl;
    return newLight;
}

// Add an ambient light (color + intensity)
void Light::addAmbientLight(const glm::vec4& color, float intensity) {
    createLight(LightType::AMBIENT, color, intensity);
}

// Add a point light (position + color + intensity)
void Light::addPointLight(const glm::vec3& position, const glm::vec4& color, float intensity) {
    LightData* light = createLight(LightType::POINT, color, intensity);
    light->position = position;
}

// Add a directional light (direction + color + intensity)
void Light::addDirectionalLight(const glm::vec3& direction, const glm::vec4& color, float intensity) {
    LightData* light = createLight(LightType::DIRECTIONAL, color, intensity);
    light->direction = direction;
}

// Add a spotlight (position + direction + color + intensity + cutoffs)
void Light::addSpotlight(const glm::vec3& position, const glm::vec3& direction, const glm::vec4& color, float intensity, float cutOff, float outerCutOff) {
    LightData* light = createLight(LightType::SPOTLIGHT, color, intensity);
    light->position = position;
    light->direction = direction;
    light->cutOff = cutOff;
    light->outerCutOff = outerCutOff;
}

// Print light information (for debugging purposes)
void Light::printLightInfo() const {
    for (const LightData* light : lights) {
        std::cout << "Light Type: ";
        switch (light->type) {
            case LightType::AMBIENT:
                std::cout << "Ambient";
                break;
            case LightType::POINT:
                std::cout << "Point";
                std::cout << ", Position: (" << light->position.x << ", " << light->position.y << ", " << light->position.z << ")";
                break;
            case LightType::DIRECTIONAL:
                std::cout << "Directional";
                std::cout << ", Direction: (" << light->direction.x << ", " << light->direction.y << ", " << light->direction.z << ")";
                break;
            case LightType::SPOTLIGHT:
                std::cout << "Spotlight";
                std::cout << ", Position: (" << light->position.x << ", " << light->position.y << ", " << light->position.z << ")";
                std::cout << ", Direction: (" << light->direction.x << ", " << light->direction.y << ", " << light->direction.z << ")";
                std::cout << ", CutOff: " << light->cutOff;
                std::cout << ", OuterCutOff: " << light->outerCutOff;
                break;
        }
        std::cout << ", Color: (" << light->color.r << ", " << light->color.g << ", " << light->color.b << ", " << light->color.a << ")";
        std::cout << ", Intensity: " << light->intensity << std::endl;
    }
}

// Use the light in the shader
void Light::useLight(Shader& shader, Camera& camera) {
    // Activate the shader
    shader.Use();

    // Set the view position
    shader.SetVector3f("viewPos", camera.Position);
    // Set the number of lights
    shader.SetInteger("lightCount", static_cast<int>(lights.size()));
    //log number of lights
    //std::cout << "Number of lights: " << lights.size() << std::endl;

    for (unsigned int i = 0; i < lights.size(); i++) {
        std::string lightType = "lights[" + std::to_string(i) + "].type";
        std::string lightColor = "lights[" + std::to_string(i) + "].color";
        std::string lightPosition = "lights[" + std::to_string(i) + "].position";
        std::string lightDirection = "lights[" + std::to_string(i) + "].direction";
        std::string lightIntensity = "lights[" + std::to_string(i) + "].intensity";
        std::string lightCutOff = "lights[" + std::to_string(i) + "].cutOff";
        std::string lightOuterCutOff = "lights[" + std::to_string(i) + "].outerCutOff";

        // Update the type casting to match the new enum values
        shader.SetInteger(lightType.c_str(), static_cast<int>(lights[i]->type));

        // Send light color, position, direction, intensity, and cutoff values
        shader.SetVector4f(lightColor.c_str(), lights[i]->color);
        
        // Set position and direction only for applicable light types
        if (lights[i]->type == LightType::POINT || lights[i]->type == LightType::SPOTLIGHT) {
            shader.SetVector3f(lightPosition.c_str(), lights[i]->position);
        }
        
        if (lights[i]->type == LightType::DIRECTIONAL || lights[i]->type == LightType::SPOTLIGHT) {
            shader.SetVector3f(lightDirection.c_str(), lights[i]->direction);
        }

        shader.SetFloat(lightIntensity.c_str(), lights[i]->intensity);
        
        // Set cutoff values only for spotlight type
        if (lights[i]->type == LightType::SPOTLIGHT) {
            shader.SetFloat(lightCutOff.c_str(), lights[i]->cutOff);
            shader.SetFloat(lightOuterCutOff.c_str(), lights[i]->outerCutOff);
        }
    }

}

// Set the types of lights
void Light::setType(LightType type) {
    for (LightData* light : lights) {
        light->type = type;
    }
}

// Set the color of the lights
void Light::setColor(glm::vec4 color) {
    for (LightData* light : lights) {
        light->color = color;
    }
}

// Set the intensity of the lights
void Light::setIntensity(float intensity) {
    for (LightData* light : lights) {
        light->intensity = intensity;
    }
}

// Set the positions of the lights
void Light::setPosition(glm::vec3 position, int i) {
    //set position of the light i 
    lights[i]->position = position;
}

// Set the directions of the lights
void Light::setDirection(glm::vec3 direction) {
    for (LightData* light : lights) {
        light->direction = direction;
    }
}

// Set the cutoff values of the lights
void Light::setCutOff(float cutOff) {
    for (LightData* light : lights) {
        light->cutOff = cutOff;
    }
}

// Set the outer cutoff values of the lights
void Light::setOuterCutOff(float outerCutOff) {
    for (LightData* light : lights) {
        light->outerCutOff = outerCutOff;
    }
}

//get lights vector and take the int i
Light::LightData* Light::getLight(int i) {
    return lights[i];
}

//get position of the light i
glm::vec3 Light::getPosition(int i) {
    return lights[i]->position;
}