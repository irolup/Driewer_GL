#include "terrain.h"

Terrain::Terrain(float gridSize) : gridSize(gridSize), width(0), height(0), rez(0.0f), numStrips(0), numTrisPerStrip(0) {
    generateTerrain(gridSize);
    loadTextures();
    setup();
}

void Terrain::generateTerrain(float gridSize) {
    // Load the heightmap
    this->gridSize = gridSize;
    int nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("texture/terrain/disp.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        float yScale = 32.0f / 255.0f;
        float yShift = 15.0f;
        rez = gridSize;
        unsigned bytesPerPixel = nrChannels;

        // Populate vertices and UV coordinates
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                unsigned char* pixelOffset = data + (j + width * i) * bytesPerPixel;
                unsigned char y = pixelOffset[0];

                // Vertex Position
                vertices.push_back(glm::vec3(-height / 2.0f + height * i / (float)height, (int)y * yScale - yShift, -width / 2.0f + width * j / (float)width));

                // UV Coordinates
                float u = static_cast<float>(j) / (width - 1);
                float v = static_cast<float>(i) / (height - 1);
                uvs.push_back(glm::vec2(u, v)); // Assuming you have a vector for UVs
            }
        }
        std::cout << "Loaded " << vertices.size() << " vertices" << std::endl;
        stbi_image_free(data);

        // Populate indices
        for (unsigned i = 0; i < height - 1; i += rez) {
            for (unsigned j = 0; j < width; j += rez) {
                for (unsigned k = 0; k < 2; k++) {
                    indices.push_back(j + width * (i + k * rez));
                }
            }
        }
        std::cout << "Loaded " << indices.size() << " indices" << std::endl;

        numStrips = (height - 1) / rez;
        numTrisPerStrip = (width / rez) * 2 - 2;
        //store the height and width of the terrain
        this->height = height;
        this->width = width;

        std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
        std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;
    } else {
        std::cout << "Failed to load heightmap" << std::endl;
    }
}

void Terrain::loadTextures() {
    // Load all PBR textures (diffuse, normal, metallic, roughness, AO, and displacement)
    glGenTextures(1, &texture_diffuse);
    glBindTexture(GL_TEXTURE_2D, texture_diffuse);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Assuming the first path is the diffuse texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("texture/terrain/diff.jpg", &width, &height, &nrChannels, 0);
    //unsigned char* data = stbi_load(texturePaths[0].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture diffuse" << std::endl;
    }
    stbi_image_free(data);

    //load all other textures in a similar manner...
    glGenTextures(1, &texture_normal);
    glBindTexture(GL_TEXTURE_2D, texture_normal);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("texture/terrain/norm.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture normal" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture_metalllic);
    glBindTexture(GL_TEXTURE_2D, texture_metalllic);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("texture/terrain/met.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture metallic" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture_roughness);
    glBindTexture(GL_TEXTURE_2D, texture_roughness);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("texture/terrain/rough.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture roughness" << std::endl;
    }

    stbi_image_free(data);

    glGenTextures(1, &texture_ao);
    glBindTexture(GL_TEXTURE_2D, texture_ao);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("texture/terrain/ao.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture AO" << std::endl;
    }
    stbi_image_free(data);

    //add textures to vector
    textures.push_back(texture_diffuse);
    textures.push_back(texture_normal);
    textures.push_back(texture_metalllic);
    textures.push_back(texture_roughness);
    textures.push_back(texture_ao);


    //glGenTextures(1, &texture_disp);
    //glBindTexture(GL_TEXTURE_2D, texture_disp);
    //// Set texture parameters
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
    //data = stbi_load("texture/terrain/disp.jpg", &width, &height, &nrChannels, 0);
    //if (data) {
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    //    glGenerateMipmap(GL_TEXTURE_2D);
    //} else {
    //    std::cout << "Failed to load texture displacement" << std::endl;
    //}
    //stbi_image_free(data);
}

void Terrain::setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Bind and set vertex buffer data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Allocate space for both vertices and UVs
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3) + uvs.size() * sizeof(glm::vec2), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), uvs.size() * sizeof(glm::vec2), uvs.data());

    // Bind and set element buffer data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // Set UV attribute pointer
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)(vertices.size() * sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind the VAO
}

void Terrain::draw(Shader& shader, Camera& camera) {
    shader.Use();
    glBindVertexArray(VAO);
    // Bind textures before drawing
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, texture_diffuse);
    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }
    // Set shader uniforms

    glm::mat4 model = glm::mat4(1.0f);
    shader.SetMatrix4("model", model);
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetMatrix4("view", view);
    glm::mat4 projection = camera.GetProjectionMatrix();
    shader.SetMatrix4("projection", projection);

    shader.SetInteger("texture_diffuse", 0);
    
    glBindVertexArray(VAO);

    //if wireframe is enabled
    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    

    for (unsigned strip = 0; strip < numStrips; strip++) {
        glDrawElements(GL_TRIANGLE_STRIP, numTrisPerStrip + 2, GL_UNSIGNED_INT, 
                       (void*)(sizeof(unsigned) * (numTrisPerStrip + 2) * strip)); // Offset to starting index
    }
    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

std::string Terrain::getInfo() const {
    return "Terrain with heightmap and PBR textures.";
}

//function to return the indices positions forcollision detection
std::vector<glm::vec3> Terrain::getVertices() {
    return vertices;
}

float Terrain::getGridSize() const {
    return gridSize;
}

// Sets a new grid size and regenerates the terrain
void Terrain::setGridSize(float newGridSize) {
    if (newGridSize > 0.0f && newGridSize != gridSize) {
        generateTerrain(newGridSize);
        setup();
    }
}

float Terrain::getHeightAt(float x, float z) {
    // Convert the world coordinates (x, z) to grid indices
    int row = static_cast<int>((z + (height / 2.0f)) / (gridSize * (height / static_cast<float>(height))));
    int col = static_cast<int>((x + (width / 2.0f)) / (gridSize * (width / static_cast<float>(width))));

    // Ensure the indices are within the valid range
    if (row < 0 || row >= height || col < 0 || col >= width) {
        std::cout << "Coordinates out of bounds!" << std::endl;
        return 0.0f; // or some default height
    }

    // Calculate the index of the vertex based on the row and column
    int index = col + (row * width);
    
    // Return the height of the terrain at the specified coordinates
    return vertices[index].y;
}

// Function to get the terrain width
int Terrain::getTerrainWidth() const {
    return width; // Return the stored width
}

// Function to get the terrain height
int Terrain::getTerrainHeight() const {
    return height; // Return the stored height
}