#include "modelLoader.h"

ModelLoader::ModelLoader() : vao(0), hasAnimation(false) {
    //animation duration and uration to 0


}

ModelLoader::~ModelLoader() {
    //for (auto& vbo : vbos) {
    //    glDeleteBuffers(1, &vbo.second);
    //}
    //glDeleteVertexArrays(1, &vao);
}

bool ModelLoader::loadModel(const char* filename) {
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool isBinary = std::string(filename).substr(std::string(filename).find_last_of(".") + 1) == "glb";
    bool res = isBinary ? loader.LoadBinaryFromFile(&model, &err, &warn, filename) :
                          loader.LoadASCIIFromFile(&model, &err, &warn, filename);

    if (!warn.empty()) std::cout << "WARN: " << warn << std::endl;
    if (!err.empty()) std::cout << "ERR: " << err << std::endl;
    if (!res) std::cout << "Failed to load glTF: " << filename << std::endl;
    else std::cout << "Loaded glTF: " << filename << std::endl;

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

                        // Flip the image vertically
                        int rowSize = image.width * image.component; // Row size in bytes
                        for (int y = 0; y < image.height / 2; ++y) {
                            // Pointers to the rows to be swapped
                            unsigned char* topRow = &image.image[y * rowSize];
                            unsigned char* bottomRow = &image.image[(image.height - 1 - y) * rowSize];
                
                            // Swap the rows
                            for (int x = 0; x < rowSize; ++x) {
                                std::swap(topRow[x], bottomRow[x]);
                            }
                        }


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

    //for (size_t i = 0; i < animation.boneTransforms.size(); ++i) {
    //    model_ *= animation.boneTransforms[i];
    //}

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

void ModelLoader::loadAnimation(int index) {
    if (model.animations.empty()) {
        std::cout << "No animations found in model." << std::endl;
        return;
    }
    
    // Get the animation at index
    const tinygltf::Animation& anim = model.animations[index];
    float maxTime = 0.0f;
    for (const auto& sampler : anim.samplers) {
        const tinygltf::Accessor& inputAccessor = model.accessors[sampler.input];
        const float* inputData = reinterpret_cast<const float*>(&model.buffers[inputAccessor.bufferView].data[inputAccessor.byteOffset]);
        maxTime = std::max(maxTime, inputData[inputAccessor.count - 1]);
    }
    animation.duration = maxTime;

    // Allocate memory for bone transforms based on the number of joints in the model
    animation.boneTransforms.resize(model.nodes.size(), glm::mat4(1.0f));

    // Initialize the current time to zero
    animation.currentTime = 0.0f;
    hasAnimation = true;

    // Print animation information
    std::cout << "Animation: " << anim.name << std::endl;
    std::cout << "Duration: " << animation.duration << "s" << std::endl;
    std::cout << "Nodes: " << model.nodes.size() << std::endl;
    std::cout << "Samplers: " << anim.samplers.size() << std::endl;
    std::cout << "Channels: " << anim.channels.size() << std::endl;
}

void ModelLoader::updateAnimation(float dt) {
    if (!hasAnimation) {
        return;
    }

    animation.currentTime += dt;
    if (animation.currentTime > animation.duration) {
        animation.currentTime = 0.0f;
    }

    // Update the bone transforms based on the current time
    for (const auto& channel : model.animations[0].channels) {
        const tinygltf::AnimationChannel& animChannel = channel;
        const tinygltf::AnimationSampler& sampler = model.animations[0].samplers[animChannel.sampler];

        // Get accessors for input (time) and output (value)
        const tinygltf::Accessor& inputAccessor = model.accessors[sampler.input];
        const tinygltf::Accessor& outputAccessor = model.accessors[sampler.output];
        
        const float* inputData = reinterpret_cast<const float*>(&model.buffers[inputAccessor.bufferView].data[inputAccessor.byteOffset]);
        const float* outputData = reinterpret_cast<const float*>(&model.buffers[outputAccessor.bufferView].data[outputAccessor.byteOffset]);

        const size_t inputCount = inputAccessor.count;
        const size_t outputCount = outputAccessor.count;

        // Find the time values that surround the current time
        size_t index = 0;
        while (index < inputCount - 1 && inputData[index + 1] < animation.currentTime) {
            ++index;
        }

        // If index is the last input value, use the last output value
        if (index >= outputCount) {
            index = outputCount - 1;
        }

        // Interpolate the output value for the current time
        float t = (animation.currentTime - inputData[index]) / (inputData[index + 1] - inputData[index]);
        glm::vec3 outputValue;
        if (outputAccessor.type == TINYGLTF_TYPE_VEC3) {
            // If the output is a vec3 (translation, scale, etc.)
            outputValue = glm::mix(
                glm::make_vec3(&outputData[index * 3]),
                glm::make_vec3(&outputData[(index + 1) * 3]),
                t
            );
        } else if (outputAccessor.type == TINYGLTF_TYPE_VEC4) {
            // If the output is a vec4 (rotation)
            glm::vec4 rotationStart = glm::make_vec4(&outputData[index * 4]);
            glm::vec4 rotationEnd = glm::make_vec4(&outputData[(index + 1) * 4]);
            outputValue = glm::mix(rotationStart, rotationEnd, t);
        }

        // Apply the output value to the corresponding node
        int nodeIndex = animChannel.target_node;
        if (nodeIndex >= 0 && nodeIndex < animation.boneTransforms.size()) {
            // Assuming translation for simplicity. Apply other transformations similarly.
            if (animChannel.target_path == "translation") {
                animation.boneTransforms[nodeIndex] = glm::translate(glm::mat4(1.0f), outputValue);
            } else if (animChannel.target_path == "rotation") {
                // Rotation needs to be converted from quaternion to mat4
                glm::quat rotation = glm::normalize(glm::make_quat(&outputValue.x));
                animation.boneTransforms[nodeIndex] = glm::mat4_cast(rotation);
            } else if (animChannel.target_path == "scale") {
                animation.boneTransforms[nodeIndex] = glm::scale(glm::mat4(1.0f), outputValue);
            }
        }
    }
}



glm::vec3 ModelLoader::getPosition() const {
    return position;
}