#include "modelLoader.h"

ModelLoader::ModelLoader() : vao(0) {}

ModelLoader::~ModelLoader() {
    //for (auto& vbo : vbos) {
    //    glDeleteBuffers(1, &vbo.second);
    //}
    //glDeleteVertexArrays(1, &vao);
}

bool ModelLoader::loadModel(const char* filename) {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cout << "ERR: " << err << std::endl;
    }

    if (!res)
        std::cout << "Failed to load glTF: " << filename << std::endl;
    else
        std::cout << "Loaded glTF: " << filename << std::endl;

    return res;
}

void ModelLoader::bindMesh(tinygltf::Mesh& mesh) {
    for (size_t i = 0; i < model.bufferViews.size(); ++i) {
        const tinygltf::BufferView& bufferView = model.bufferViews[i];
        if (bufferView.target == 0) {
            std::cout << "WARN: bufferView.target is zero" << std::endl;
            continue;
        }

        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

        GLuint vbo;
        glGenBuffers(1, &vbo);
        vbos[i] = vbo;
        glBindBuffer(bufferView.target, vbo);

        glBufferData(bufferView.target, bufferView.byteLength,
                     &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
    }

    for (auto& primitive : mesh.primitives) {
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        for (auto& attrib : primitive.attributes) {
            tinygltf::Accessor accessor = model.accessors[attrib.second];
            int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
            glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

            int size = (accessor.type != TINYGLTF_TYPE_SCALAR) ? accessor.type : 1;
            int vaa = -1;
            if (attrib.first == "POSITION") vaa = 0;
            if (attrib.first == "NORMAL") vaa = 1;
            if (attrib.first == "TEXCOORD_0") vaa = 2;
            if (vaa > -1) {
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                                      accessor.normalized ? GL_TRUE : GL_FALSE,
                                      byteStride, BUFFER_OFFSET(accessor.byteOffset));
            } else {
                std::cout << "vaa missing: " << attrib.first << std::endl;
            }
        }
        //store the in vao

        if (!model.textures.empty()) {
            for (size_t i = 0; i < model.textures.size(); ++i) {
                tinygltf::Texture& tex = model.textures[i];
                if (tex.source > -1) {
                    GLuint texid;
                    glGenTextures(1, &texid);
                    textures_model.push_back(texid); // Store texture IDs
                    tinygltf::Image& image = model.images[tex.source];
                    glBindTexture(GL_TEXTURE_2D, texid);
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    GLenum format = (image.component == 3) ? GL_RGB : GL_RGBA;
                    GLenum type = (image.bits == 8) ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT;
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                                 format, type, &image.image.at(0));
                }
            }
        }
    }

    //unbind the buffer
    glBindVertexArray(0);
}

void ModelLoader::bindModelNodes(tinygltf::Node& node) {
    if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
        bindMesh(model.meshes[node.mesh]);
    }

    for (size_t i = 0; i < node.children.size(); i++) {
        bindModelNodes(model.nodes[node.children[i]]);
    }
}

void ModelLoader::bindModel() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        bindModelNodes(model.nodes[scene.nodes[i]]);
    }

    glBindVertexArray(0);
}

void ModelLoader::drawMesh(const tinygltf::Mesh& mesh) const {
    for (auto& primitive : mesh.primitives) {
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));
        glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType,
                       BUFFER_OFFSET(indexAccessor.byteOffset));
    }
}

void ModelLoader::drawModelNodes(const tinygltf::Node& node) const {
    if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
        drawMesh(model.meshes[node.mesh]);
    }
    for (size_t i = 0; i < node.children.size(); i++) {
        drawModelNodes(model.nodes[node.children[i]]);
    }
}

void ModelLoader::drawModel(Shader& shader, Camera& camera) {
    shader.Use();

    glm::mat4 model_ = glm::mat4(1.0f);
    model_ = glm::translate(model_, getPosition());  // Assuming getPosition() is a method that returns the model's position
    model_ = glm::scale(model_, scale);  // Use the appropriate scale for your model

    shader.SetMatrix4("model", model_);
    
    glm::mat4 projection = camera.GetProjectionMatrix();
    shader.SetMatrix4("projection", projection);
    
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetMatrix4("view", view);

    glm::vec3 viewPos = camera.Position;
    shader.SetVector3f("viewPos", viewPos);

    shader.SetInteger("texture_diffuse", 0);
    //shader.SetInteger("texture_normal", 1);

    //bind texture but texture 0 is normal, 1 is diffuse and 2 metal
    
    
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, textures_model[1]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures_model[0]);
    
    //LOAD CURRENTLY THE WRONG TEXTURE    
    
    


    glBindVertexArray(vao);

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        drawModelNodes(model.nodes[scene.nodes[i]]);
    }

    glBindVertexArray(0);
}

void ModelLoader::dbgModel() const {
    for (const auto& mesh : model.meshes) {
        std::cout << "mesh : " << mesh.name << std::endl;
        for (const auto& primitive : mesh.primitives) {
            const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
            std::cout << "indexaccessor: count " << indexAccessor.count
                      << ", type " << indexAccessor.componentType << std::endl;
            const tinygltf::Material& mat = model.materials[primitive.material];
            for (const auto& mats : mat.values) {
                std::cout << "mat : " << mats.first.c_str() << std::endl;
            }
            for (const auto& image : model.images) {
                std::cout << "image name : " << image.uri
                          << "  size : " << image.image.size()
                          << "  w/h : " << image.width << "/" << image.height
                          << std::endl;
            }
            std::cout << "indices : " << primitive.indices << std::endl;
            std::cout << "mode : " << primitive.mode << std::endl;
            for (const auto& attrib : primitive.attributes) {
                std::cout << "attribute : " << attrib.first.c_str() << std::endl;
            }
        }
    }
}

glm::vec3 ModelLoader::getPosition() const {
    return position;
}