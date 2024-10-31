#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H
#include "../include/tiny_gltf.h"
#include "../include/stb_image.h"
#include "../include/stb_image_write.h"
#include "../glad/glad.h"
#include "../shaders/shader.h"
#include "../camera/camera.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

class ModelLoader {
public:
    ModelLoader();
    ~ModelLoader();

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(10.0f);

    bool loadModel(const char* filename);
    void bindModel();
    void drawModel(Shader& shader, Camera& camera);
    void dbgModel() const;

private:
    void bindMesh(tinygltf::Mesh& mesh);
    void bindModelNodes(tinygltf::Node& node);
    void drawMesh(const tinygltf::Mesh& mesh) const;
    void drawModelNodes(const tinygltf::Node& node) const;
    glm::vec3 getPosition() const;

    tinygltf::Model model;
    GLuint vao;
    std::map<int, GLuint> vbos;
    std::vector<unsigned int> textures_model;
};

#endif