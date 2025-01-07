#include "application.h"
#include "../resources/resource_manager.h"
#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "../include/stb_image_write.h"
#include "../include/tiny_gltf.h"
#include "gbuffer.h"
#include <unistd.h>
#include <glm/gtx/string_cast.hpp>


Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_MENU), Rendermode(DEFERRED_RENDERING), Keys(), KeysProcessed(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete myCamera;
    delete cube;
    delete plane;
    delete sphere;
    //for (int i = 0; i < primitives.size(); i++) {
    //    delete primitives[i];
    //}
}

void Game::Init()
{
    glEnable(GL_DEPTH_TEST);

    ResourceManager::LoadShader("shaders/hitbox.vs", "shaders/hitbox.fs", nullptr, "hitbox");
    hitboxShader = ResourceManager::GetShader("hitbox");

    ResourceManager::LoadShader("shaders/lights.vs", "shaders/lights.fs", nullptr, "lights");
    lightShader = ResourceManager::GetShader("lights");

    ResourceManager::LoadShader("shaders/PBR.vs", "shaders/PBR.fs", nullptr, "PBR");
    PBR = ResourceManager::GetShader("PBR");

    ResourceManager::LoadShader("shaders/PBR_notext.vs", "shaders/PBR_notext.fs", nullptr, "PBR_notext");
    PBR_notext = ResourceManager::GetShader("PBR_notext");

    ResourceManager::LoadShader("shaders/fxaa.vs", "shaders/fxaa.fs", nullptr, "fxaa_shader");
    fxaaShader = ResourceManager::GetShader("fxaa_shader");

    ResourceManager::LoadShader("shaders/taa.vs", "shaders/taa.fs", nullptr, "taa");
    taaShader = ResourceManager::GetShader("taa");

    ResourceManager::LoadShader("shaders/msaa.vs", "shaders/msaa.fs", nullptr, "msaa");
    msaaShader = ResourceManager::GetShader("msaa");

    ResourceManager::LoadShader("shaders/smaa.vs", "shaders/smaa.fs", nullptr, "smaa");
    smaaShader = ResourceManager::GetShader("smaa");

    ResourceManager::LoadShader("shaders/height.vs", "shaders/height.fs", nullptr, "height");
    terrainShader = ResourceManager::GetShader("height");

    ResourceManager::LoadShader("shaders/animation.vs", "shaders/animation.fs", nullptr, "animation");
    animationShader = ResourceManager::GetShader("animation");

    ResourceManager::LoadShader("shaders/SSGI/gbufferSSGI.vs", "shaders/SSGI/gbufferSSGI.fs", nullptr, "gbuffer");
    Gbuffer_shader = ResourceManager::GetShader("gbuffer");

    ResourceManager::LoadShader("shaders/SSGI/lightPass.vs", "shaders/SSGI/lightPass.fs", nullptr, "lightPass");
    lightpass = ResourceManager::GetShader("lightPass");

    ResourceManager::LoadShader("shaders/default.vs", "shaders/default.fs", nullptr, "default");
    defaultShader = ResourceManager::GetShader("default");

    ResourceManager::LoadShader("shaders/SSGI/ssao.vs", "shaders/SSGI/ssao.fs", nullptr, "ssao");
    ssaoshader = ResourceManager::GetShader("ssao");

    ResourceManager::LoadShader("shaders/SSGI/ssao_blur.vs", "shaders/SSGI/ssao_blur.fs", nullptr, "ssao_blur");
    ssaoblurshader = ResourceManager::GetShader("ssao_blur");

    ResourceManager::LoadShader("shaders/SSGI/lightPass.vs", "shaders/SSGI/lightPass_ssao.fs", nullptr, "lightPassSSAO");
    lightpassSSAO = ResourceManager::GetShader("lightPassSSAO");

    //shadows
    ResourceManager::LoadShader("shaders/shadows/shadow_mapping_depth.vs", "shaders/shadows/shadow_mapping_depth.fs", nullptr, "shadowdepth");
    simpleDepthShader = ResourceManager::GetShader("shadowdepth");

    ResourceManager::LoadShader("shaders/shadows/point_shadows_depth.vs", "shaders/shadows/point_shadows_depth.fs", "shaders/shadows/point_shadows_depth.gs", "simpleDepthShaderPoint");
    simpleDepthShaderPoint = ResourceManager::GetShader("simpleDepthShaderPoint");

    ResourceManager::LoadShader("shaders/shadows/pbr_shadows.vs", "shaders/shadows/pbr_shadows.fs", nullptr, "pbr_shadows");
    pbr_shadows = ResourceManager::GetShader("pbr_shadows");

    antialiasing = new Antialiasing(Width, Height, Antialiasing::Type::NONE);


    //here we initialize all the textures
    GBuffer_ = new GBuffer(Width, Height, GBuffer::Type::BASIC);
    ssao = new ssaoBuffer(Width, Height, ssaoshader, ssaoblurshader);

    //cam with width and height and position
    myCamera = new Camera(Width, Height, glm::vec3(0.0f, 20.0f, 2.0f));


    //Cube object
    cube = new Cube();
    cube->collisionEnabled = true;
    cube->isStatic = false;
    cube->setPosition(glm::vec3(0.0f, 7.0f, 0.0f));
    cube->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
    primitives.push_back(cube);

    plane = new Plane();
    plane->collisionEnabled = true;
    plane->isStatic = true;
    plane->setScale(glm::vec3(10.0f, 1.0f, 10.0f));
    plane->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    primitives.push_back(plane);
    
    
    //primitives.push_back(sphere);

    drawHitbox = false;

    //for pointlight
    sphere_light = new Sphere();
    sphere_light->collisionEnabled = false;
    sphere_light->isStatic = true;
    sphere_light->setPosition(glm::vec3(-5.0f, 5.0f, -5.0));
    primitives.push_back(sphere_light);

    //for spotlight
    sphere_light = new Sphere();
    sphere_light->collisionEnabled = false;
    sphere_light->isStatic = true;
    sphere_light->setPosition(glm::vec3(5.0f, 5.0f, 5.0f));
    primitives.push_back(sphere_light);

    //terrain
    terrain = new Terrain(1.0f);

    //light.addPointLight(glm::vec3(-5.0f, 5.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 10.0f);
    ////
    //light.addSpotlight(glm::vec3(5.0f, 5.0f, 5.0f), glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 10.0f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(25.0f)));

    //add another spotlight pointing at the cube
    //light.addSpotlight(glm::vec3(-5.0f, 5.0f, -5.0f), glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f)), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 10.0f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(25.0f)));

    //directional light pointing at the cube at position 0,0,0
    //light.addDirectionalLight(glm::vec3(-6.0f, 8.0f, -6.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec4(3.0f, 3.0f, 3.0f, 3.0f), 30.0f);

    //point light
    light.addPointLight(glm::vec3(-5.0f, 5.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 10.0f);

    //add cube primitive to indicate the directionnal light but one behind the light
    //cube = new Cube();
    //cube->collisionEnabled = false;
    //cube->isStatic = true;
    //cube->setPosition(glm::vec3(-2.0f, 4.0f, -1.0f));
    //cube->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
    //primitives.push_back(cube);


    //Collsion test
    player = new Player(glm::vec3(0.0f, 10.0f, 2.0f), glm::vec3(1.0f, 2.0f, 1.0f), *myCamera);
    //primitives.push_back(player);

    //vertices for collision detection
    vertices_terrain = terrain->getVertices();

    //make string from ../models/wooden_axe_02_1k.gltf
    //std::string filename = "models/wooden_axe_02_1k.gltf";
    //load lemon
    std::string filename = "models/lemon_1k.gltf";

    if(modelLoader.loadModel(filename.c_str()))
    {
        modelLoader.bindModel();
        std::cout << "Model loaded" << std::endl;
        //load animation
        //modelLoader.loadAnimation(0);
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
    }
    //set position
    modelLoader.position = glm::vec3(0.8f, 0.5f, 0.8f);
    //reduce scale
    //modelLoader.scale = glm::vec3(0.1f, 0.1f, 0.1f);

    //load with assimp
    model_animation = Model("models/michel.fbx");
 
    animation = Animation("models/michel.fbx", &model_animation);
    animator = Animator(&animation);


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    
    // Setup Platform/Renderer backends
     // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();


}

void Game::Update(float dt)
{
    collision.update(primitives);
    collision.updatePlayer(player, primitives, vertices_terrain);

    //update player
    player->update(dt);

    //update animation
    animator.UpdateAnimation(dt);

    //std::cout << "PLayer colliding with primitives" << collision.getCollisionWithPlayerwithPrimitives() << std::endl;
    //std::cout << "PLayer colliding with terrain" << collision.getCollisionWithPlayerwithTerrain() << std::endl;

}

void Game::Render()
{
    //imgui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //ImGui::ShowDemoWindow();
    ImGui::Begin("Main Window", &mainwindow, ImGuiWindowFlags_MenuBar);
    ImGui::Text("Menu");
    //butoon for forward rendering or deferred rendering
    if (ImGui::Button("Forward Rendering")){
        Rendermode = FORWARD_RENDERING;
    }
    if (ImGui::Button("Deferred Rendering")){
        Rendermode = DEFERRED_RENDERING;
    }
    if (ImGui::Button("Other Rendering")){
        Rendermode = OTHER;
    }
    if (ImGui::Button("Shadows")){
        Rendermode = SHADOWS;
    }
    int i = 0;
    //move spot light with ->setPosition
    if (ImGui::SliderFloat("Move Spot Light", &sliderx, -5.0f, 5.0f)){
        
        float posx = light.getPosition(i).x;
        float posy = light.getPosition(i).y;
        float posz = light.getPosition(i).z;

        light.setPosition(glm::vec3(sliderx, posy, posz), i);
        //text for position of the light x, y, z
    }
    //move in z
    if (ImGui::SliderFloat("Move Spot Light Z", &sliderz, -5.0f, 5.0f)){
        
        float posx = light.getPosition(i).x;
        float posy = light.getPosition(i).y;
        float posz = light.getPosition(i).z;

        light.setPosition(glm::vec3(posx, posy, sliderz), i);
        //text for position of the light x, y, z
    }



    ImGui::Text("Position of the light x: %f y: %f z: %f", light.getPosition(i).x, light.getPosition(i).y, light.getPosition(i).z);

    //Position of the camera
    ImGui::Text("Position of the player x: %f y: %f z: %f", myCamera->Position.x, myCamera->Position.y, myCamera->Position.z);

    //slider for sample radius
    if (ImGui::SliderFloat("Sample ao", &aoSlider, 0.0f, 1.0f)){
        ao = aoSlider;
    }


    //end
    ImGui::End();

    if (this->Rendermode == FORWARD_RENDERING){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (fxaaActive)
        {
            antialiasing->BindFramebuffer();
        }

        if(texturesActive)
        {
            light.useLight(PBR, *myCamera);

            terrain->draw(terrainShader, *myCamera);
            for (int i = 0; i < primitives.size(); i++)
            {
                primitives[i]->draw(PBR, *myCamera);
            }
            modelLoader.drawModel(PBR, *myCamera);

            //animation NEED TO PUT THOSE IN A FUNCTION INSIDE THE MODEL CLASS
            animationShader.Use();
            auto transforms = animator.GetFinalBoneMatrices();
	    	//for loop transforms
	    	for (unsigned int i = 0; i < transforms.size(); i++){
	    		animationShader.SetMatrix4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
	    	}
            model_animation.Draw(animationShader, *myCamera);

        }
        else
        {
            light.useLight(PBR_notext, *myCamera);
            for (int i = 0; i < primitives.size(); i++)
            {
                primitives[i]->draw(PBR_notext, *myCamera);
            }
        }

        //render SSGI here with the RenderWithShaderfunction

        if (fxaaActive)
        {
            antialiasing->UpdateHistoryBuffer(*myCamera);
            //antialiasing->RenderWithShader(taaShader, *myCamera);
            antialiasing->RenderWithShader(fxaaShader, *myCamera);
            //antialiasing->RenderWithShader(smaaShader, *myCamera);
        }

    } else if (this->Rendermode == DEFERRED_RENDERING) {
        //deferred rendering
        //1. geometry pass: render scene's geometry/color data into gbuffer
        GBuffer_->BindFramebuffer();
        //draw scene

        terrain->draw(terrainShader, *myCamera);
        //modelLoader.drawModel(PBR, *myCamera);
        //animationShader.Use();
        //auto transforms = animator.GetFinalBoneMatrices();
        //for (unsigned int i = 0; i < transforms.size(); i++){
	    //		animationShader.SetMatrix4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
	    //	}
        //model_animation.Draw(animationShader, *myCamera);

        for (int i = 0; i < primitives.size(); i++)
        {
            primitives[i]->draw(Gbuffer_shader, *myCamera);
        }
        GBuffer_->UnbindFramebuffer();
        //2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GBuffer_->RenderWithShader(lightpass, *myCamera, ao);
        light.useLight(lightpass, *myCamera);
        //render quad
        //GBuffer_->renderQuad();
        
        //2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
        glBindBuffer(GL_READ_FRAMEBUFFER, GBuffer_->framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        

    } else if (this->Rendermode == OTHER) {
        GBuffer_->BindFramebuffer();
        //draw scene

        terrain->draw(terrainShader, *myCamera);

        for (int i = 0; i < primitives.size(); i++)
        {
            primitives[i]->draw(Gbuffer_shader, *myCamera);
        }
        GBuffer_->UnbindFramebuffer();

        //generate ssao texture
        ssao->RenderWithSSAO(ssaoshader, *myCamera);

        //blur ssao texture
        ssao->RenderWithSSAOBlur(ssaoblurshader, *myCamera);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GBuffer_->RenderWithShaderSSAO(lightpassSSAO, *myCamera, ssao->getSSAOTexture());
        light.useLight(lightpassSSAO, *myCamera);
        //render quad
        //GBuffer_->renderQuad();
        
        //2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
        glBindBuffer(GL_READ_FRAMEBUFFER, GBuffer_->framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else if (this->Rendermode == SHADOWS) {
        
        // 1. render depth of scene to texture (from light's perspective)
        // - Get light projection/view matrix.
        //for each light we need to render the scene to the depth map
        for (int i = 0; i < light.getLights().size(); i++) {
            //send light count to the shader
            simpleDepthShader.Use();
            simpleDepthShader.SetInteger("lightCount", static_cast<int>(light.getLights().size()));

            std::vector<glm::mat4> shadowTransforms;
            if(light.getLight(i)->type == Light::LightType::POINT)
            {
                light.useOneLightPoint(simpleDepthShaderPoint, *myCamera, i);
                shadowTransforms = light.getLightSpaceMatricesFromPointLight(i);
            } else {
                light.useOneLight(simpleDepthShader, *myCamera, i);
            }

            unsigned int depthMapFBO = light.getLight(i)->depthMapFBO;
            glViewport(0, 0, 1024, 1024);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
            //loop over the primitives
            if(light.getLight(i)->type == Light::LightType::POINT)
            {
                simpleDepthShaderPoint.Use();
                for (int j = 0; j < shadowTransforms.size(); j++) {
                    
                    simpleDepthShaderPoint.SetMatrix4(("shadowMatrices[" + std::to_string(j) + "]").c_str(), shadowTransforms[j]);
                }
                for (int j = 0; j < primitives.size(); j++) {
                    //draw the scene
                    primitives[j]->draw(simpleDepthShaderPoint, *myCamera);
                }
            } else {
                for (int j = 0; j < primitives.size(); j++) {
                    //draw the scene
                    primitives[j]->draw(simpleDepthShader, *myCamera);
                }
            }

            
            glCullFace(GL_BACK);
            //unbind framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        //reset viewport
        glViewport(0, 0, Width, Height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //render the scene using the shadow map

        pbr_shadows.Use();
        //set projection and view matrix
        pbr_shadows.SetMatrix4("projection", myCamera->GetProjectionMatrix());
        pbr_shadows.SetMatrix4("view", myCamera->GetViewMatrix());

        pbr_shadows.SetInteger("lightCount", static_cast<int>(light.getLights().size()));
        //camera near and far plane
        pbr_shadows.SetFloat("near_plane", myCamera->GetNearPlane());
        pbr_shadows.SetFloat("far_plane", myCamera->GetFarPlane());

        for (int i = 0; i < light.getLights().size(); i++) {
            //render one light
            light.useOneLight(pbr_shadows, *myCamera, i);
            //render the scene
            for (int j = 0; j < primitives.size(); j++) {
                unsigned int shadowMap = light.getLight(i)->depthMap;
                //if pointlight
                if(light.getLight(i)->type == Light::LightType::POINT)
                {
                    primitives[j]->drawWithShadow(pbr_shadows, *myCamera, shadowMap, true);
                } else{
                    primitives[j]->drawWithShadow(pbr_shadows, *myCamera, shadowMap, false);
                }

                
                //cout debug
                std::cout << "Tried to draw with shadow" << std::endl;
                //cout id of map to see if it is the same
                std::cout << "Depth map id: " << shadowMap << std::endl;
                //check fbos
                std::cout << "Depth map fbo: " << light.getLight(i)->depthMapFBO << std::endl;
            }
        }



    }

    //imgui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_MENU)
    {
        float cameraSpeed = 4.0f * dt;
        //myCamera->ProcessJump(dt, window);
        if (this->Keys[GLFW_KEY_W] ){
            //myCamera->ProcessKeyboard(Camera_Movement::FORWARD, cameraSpeed);
            player->movePlayer(Camera_Movement::FORWARD, cameraSpeed);
        }
        if (this->Keys[GLFW_KEY_S]){
            //myCamera->ProcessKeyboard(Camera_Movement::BACKWARD, cameraSpeed);
            player->movePlayer(Camera_Movement::BACKWARD, cameraSpeed);
        }
        if (this->Keys[GLFW_KEY_A]){
            //myCamera->ProcessKeyboard(Camera_Movement::LEFT, cameraSpeed);
            player->movePlayer(Camera_Movement::LEFT, cameraSpeed);
        }
        if (this->Keys[GLFW_KEY_D]){    
            //myCamera->ProcessKeyboard(Camera_Movement::RIGHT, cameraSpeed);
            player->movePlayer(Camera_Movement::RIGHT, cameraSpeed);
        }
        //spacebar to jump
        if (this->Keys[GLFW_KEY_SPACE])
        {
            player->jump(window, dt, terrain, vertices_terrain, collision);
        }
        
        //player->jump(window, dt, terrain, vertices_terrain);

        //activated mouse
        if (this->Keys[GLFW_KEY_M]){
            isMouseEnabled = true;
            EnableMouse();
            std::cout << "Mouse is enabled" << std::endl;
        }
        if (this->Keys[GLFW_KEY_N]){
            isMouseEnabled = false;
            DisableMouse();
            std::cout << "Mouse is disabled" << std::endl;
        }
        //t and y to texture and no texture
        if (this->Keys[GLFW_KEY_T])
        {
            texturesActive = true;
            std::cout << "Textures are enabled" << std::endl;
        }
        if (this->Keys[GLFW_KEY_Y])
        {
            texturesActive = false;
            std::cout << "Textures are disabled" << std::endl;
        }
        

        //key c to make all primitives static
        if (this->Keys[GLFW_KEY_C]){
            for (int i = 0; i < primitives.size(); i++) {
                primitives[i]->isStatic = true;
            }
            std::cout << "All primitives are static" << std::endl;
        }
        //key x to make all primitives dynamic (not the plane)
        if (this->Keys[GLFW_KEY_X]){
            for (int i = 0; i < primitives.size(); i++) {
                if (primitives[i]->getInfo() != "Plane") {
                    primitives[i]->isStatic = false;
                } else {
                    primitives[i]->isStatic = true;
                }
            }
            std::cout << "All primitives are dynamic" << std::endl;
        }
        //key z to replace the primitives to their original position
        if (this->Keys[GLFW_KEY_Z]){
            for (int i = 0; i < primitives.size(); i++) {
                if (primitives[i]->getInfo() == "Plane") {
                    primitives[i]->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
                } else if //sphere not change position
                (primitives[i]->getInfo() == "Sphere") {
                    primitives[i]->setPosition(glm::vec3(0.0f, 9.0f, 0.0f));
                } else if (primitives[i]->getInfo() == "Cube") {
                    primitives[i]->setPosition(glm::vec3(0.0f, 3.0f, 0.0f));
                }
            }
            std::cout << "All primitives are back to their original position" << std::endl;
        }
        //key h to draw hitbox and j to not draw hitbox
        if (this->Keys[GLFW_KEY_H]){
            drawHitbox = true;
            std::cout << "Hitbox is enabled" << std::endl;
        }
        if (this->Keys[GLFW_KEY_J]){
            drawHitbox = false;
            std::cout << "Hitbox is disabled" << std::endl;
        }
        //1 to enable taa and 2 to disable fxaa
        if (this->Keys[GLFW_KEY_1]){
            fxaaActive = true;
            std::cout << "FXAA is enabled" << std::endl;
        }
        if (this->Keys[GLFW_KEY_2]){
            fxaaActive = false;
            std::cout << "FXAA is disabled" << std::endl;
        }
        //0 for forward rendering and 9 for deferred rendering
        if (this->Keys[GLFW_KEY_0]){
            Rendermode = FORWARD_RENDERING;
            std::cout << "Forward Rendering" << std::endl;
        }
        if (this->Keys[GLFW_KEY_9]){
            Rendermode = DEFERRED_RENDERING;
            std::cout << "Deferred Rendering" << std::endl;
        }

    }
    if (this->State == GAME_WIN)
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->State = GAME_MENU;
        }
    }
    if (this->State == GAME_ACTIVE)
    {
        
    }
}

void Game::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (!isMouseEnabled)
    {
        return;
    }
    
    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    // Calculate the offset between the current and previous positions
    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos); // Reversed since y-coordinates go from bottom to top

    // Update lastX and lastY to the current position
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    // Pass the offsets to the camera (be sure this method handles both offsets)
    myCamera->ProcessMouseMovement(xoffset, yoffset);
}

void Game::SetWindow(GLFWwindow* win) {
    window = win;
}

void Game::EnableMouse() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    isMouseEnabled = true;
}

void Game::DisableMouse() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    isMouseEnabled = false;
}

void Game::SetCameraWindowSize(unsigned int width, unsigned int height) {
    myCamera->Width = width;
    myCamera->Height = height;
    antialiasing->Update(width, height);
}

void Game::cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}