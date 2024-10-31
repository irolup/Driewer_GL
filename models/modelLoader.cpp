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
            if (attrib.first == "POSITION"){
                vaa = 0;
            } else if (attrib.first == "NORMAL"){
                vaa = 1;
            } else if (attrib.first == "TEXCOORD_0"){
                vaa = 2;
            } else if (attrib.first == "TANGENT"){
                vaa = 3;
            } else if (attrib.first == "COLOR_0"){
                vaa = 4;
            } else if (attrib.first == "JOINTS_0"){
                vaa = 5;
            } else if (attrib.first == "WEIGHTS_0"){
                vaa = 6;
            }
            
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
            int diffuseIndex = -1;
            int normalIndex = -1;
            int metallicIndex = -1;
        
            // Identify the indices for each texture type in the materials
            for (const auto& material : model.materials) {
                //std::cout << "Material: " << material.name << std::endl;
                
                // Check for diffuse texture
                auto diffuseIt = material.pbrMetallicRoughness.baseColorTexture.index;
                if (diffuseIt >= 0) {
                    diffuseIndex = diffuseIt;
                    //std::cout << "Diffuse Index: " << diffuseIndex << std::endl;
                }
                
                // Check for normal texture
                auto normalIt = material.normalTexture.index;
                if (normalIt >= 0) {
                    normalIndex = normalIt;
                    //std::cout << "Normal Index: " << normalIndex << std::endl;
                }
                
                // Check for metallic texture
                auto metallicIt = material.pbrMetallicRoughness.metallicRoughnessTexture.index;
                if (metallicIt >= 0) {
                    metallicIndex = metallicIt;
                    //std::cout << "Metallic Index: " << metallicIndex << std::endl;
                }
            }
        
            // Load textures based on specified order
            std::vector<int> loadOrder = {diffuseIndex, normalIndex, metallicIndex};
            for (size_t i = 0; i < loadOrder.size(); ++i) {
                int texIndex = loadOrder[i];
                if (texIndex >= 0 && texIndex < model.textures.size()) {
                    tinygltf::Texture& tex = model.textures[texIndex];
                    if (tex.source > -1) {
                        GLuint texid;
                        glGenTextures(1, &texid);
                        textures_model.push_back(texid); // Store texture IDs
                        tinygltf::Image& image = model.images[tex.source];


                        glBindTexture(GL_TEXTURE_2D, texid);
                        //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                        GLenum format = (image.component == 3) ? GL_RGB : GL_RGBA;
                        GLenum type = (image.bits == 8) ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT;
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                                     format, type, &image.image.at(0));
                        glGenerateMipmap(GL_TEXTURE_2D);
                        
                    }
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

    //scale is 10 but even at 1 the texture strech
    shader.SetMatrix4("model", model_);
    
    glm::mat4 projection = camera.GetProjectionMatrix();
    shader.SetMatrix4("projection", projection);
    
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetMatrix4("view", view);

    glm::vec3 viewPos = camera.Position;
    shader.SetVector3f("viewPos", viewPos);

    shader.SetVector3f("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.SetVector3f("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.SetVector3f("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    //metallic and roughness for citrus fruit
    shader.SetFloat("material.metallic", 0.0f);
    shader.SetFloat("material.roughness", 0.5f);
    shader.SetFloat("material.occlusion", 1.0f);
    shader.SetFloat("material.brightness", 1.0f);
    shader.SetVector3f("material.fresnel_ior", glm::vec3(1.5f));

    for (size_t i = 0; i < textures_model.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures_model[i]);
        //cout texture id
        //std::cout << "Texture ID: " << textures_model[i] << std::endl;
    }
//
    shader.SetInteger("texture_diffuse", 0);
    shader.SetInteger("texture_normal", 1);
    shader.SetInteger("texture_metallic", 2);
    shader.SetInteger("texture_roughness", 3);
    shader.SetInteger("texture_occlusion", 4);
    shader.SetInteger("texture_disp", 5);

    glBindVertexArray(vao);

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        drawModelNodes(model.nodes[scene.nodes[i]]);
    }
    glBindVertexArray(0);

    //unbind the texture
    for (size_t i = 0; i < textures_model.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void ModelLoader::dbgModel() const {
    std::cout << "Model Textures and Associated Images:" << std::endl;
    
    for (size_t i = 0; i < model.textures.size(); ++i) {
        const tinygltf::Texture& tex = model.textures[i];
        if (tex.source >= 0 && tex.source < model.images.size()) {
            const tinygltf::Image& image = model.images[tex.source];
            std::cout << "Texture " << i << ":"
                      << "\n- Image URI: " << image.uri
                      << "\n- Size: " << image.image.size()
                      << "\n- Dimensions: " << image.width << "x" << image.height
                      << "\n- Format: " << ((image.component == 3) ? "RGB" : "RGBA")
                      << "\n" << std::endl;
        } else {
            std::cout << "Texture " << i << " has an invalid source index: " << tex.source << std::endl;
        }
    }

    // Original mesh debugging code
    for (const auto& mesh : model.meshes) {
        std::cout << "Mesh : " << mesh.name << std::endl;
        for (const auto& primitive : mesh.primitives) {
            const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
            std::cout << "Index Accessor: count " << indexAccessor.count
                      << ", type " << indexAccessor.componentType << std::endl;
            const tinygltf::Material& mat = model.materials[primitive.material];
            for (const auto& mats : mat.values) {
                std::cout << "Material Property: " << mats.first.c_str() << std::endl;
            }
            std::cout << "Indices : " << primitive.indices << std::endl;
            std::cout << "Mode : " << primitive.mode << std::endl;
            for (const auto& attrib : primitive.attributes) {
                std::cout << "Attribute : " << attrib.first.c_str() << std::endl;
            }
        }
    }
}

glm::vec3 ModelLoader::getPosition() const {
    return position;
}