#include "CarModels.h"

#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/imgui/imgui.h"

#include <dirent.h>


/*
    Some of the code was for shadow mapping was influenced by code derived from this tutorial:
    https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
    Author: Joe de Vriez (https://twitter.com/JoeyDeVriez)
    licensed under CC BY 4.0

    Code for dynamic environment mapping was influenced by the repository in the following link:
    https://github.com/khongton/Dynamic-Cubemaps

*/


std::vector<std::string> searchDirectory(std::string path, int remove);

namespace test_model
{
    /**
     * @brief Construct a new Car Models:: Car Models object
     * 
     */
    CarModels::CarModels() 
        : m_ClearColor {0.2f, 0.3f, 0.8f, 1.0f}
    {
    }

    CarModels::CarModels(GLFWwindow* window, int width, int height) 
        : m_Window(window), WIDTH(width), HEIGHT(height)
    {
        modelInit();
    }

    /**
     * @brief Initializes the test model.
     * 
     */
    void CarModels::modelInit()
    {

        modelsVec = searchDirectory("res/models", 0);
        skyboxesVec = searchDirectory("res/textures/skyboxes", 9);
        planesVec = searchDirectory("res/planes", 0);
        objectsVec = searchDirectory("res/objects", 0);


        sort( skyboxesVec.begin(), skyboxesVec.end() );
        skyboxesVec.erase( unique( skyboxesVec.begin(), skyboxesVec.end() ), skyboxesVec.end() );


        scale = glm::vec3(1.0,1.0,1.0);

        m_Model = std::make_unique<Model>("res/models/porsche/porsche.obj", "res/models/porsche/");
        m_PlaneModel = std::make_unique<Model>("res/planes/parking_lot/parking_lot.obj", "res/planes/parking_lot/");
        m_ObjectModel = std::make_unique<Model>("res/objects/lamps/lamps.obj", "res/objects/lamps/");

        glEnable(GL_DEPTH_TEST);
        m_Camera = std::make_unique<Camera>(WIDTH, HEIGHT, 
                                            glm::vec3(2.74633, 1.87862, 5.50846),
                                            glm::vec3(-0.482962, -0.19509, -0.853632),
                                            glm::vec3(0, 1, 0)
                                            );


        m_LightCubeShader = std::make_unique<Shader>("res/shaders/lightCube.shader");
        m_LightCube = std::make_unique<Model>("res/models/lightCube/lightCube.obj");

        m_Shader = std::make_unique<Shader>("res/shaders/shadowmap.shader");
        m_SkyboxShader = std::make_unique<Shader>("res/shaders/skybox.shader");

        vector<std::string> faces1
        {
            "res/textures/skyboxes/mountain_posx.jpg",
            "res/textures/skyboxes/mountain_negx.jpg",
            "res/textures/skyboxes/mountain_posy.jpg",
            "res/textures/skyboxes/mountain_negy.jpg",
            "res/textures/skyboxes/mountain_posz.jpg",
            "res/textures/skyboxes/mountain_negz.jpg"
        };
        m_SkyboxMap = std::make_unique<CubeMap>(faces1);

        // Dynamic cubemap stuff
        glGenFramebuffers(1, &fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);	
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        for (int i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenRenderbuffers(1, &renderID);
        glBindRenderbuffer(GL_RENDERBUFFER, renderID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 2048, 2048);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, textureID, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		    cerr << "FrameBuffer isn't ok" << endl;
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //--------------------------

        // Depthmapping stuff
        //--------------------------
        m_DepthShader = std::make_unique<Shader>("res/shaders/depth.shader");

        const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
        glGenFramebuffers(1, &depthMapFBO);

        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //--------------------------
        
        m_Shader->Bind();
        m_Shader->SetUniform1i("skybox", 0);
        m_Shader->SetUniform1i("u_Texture", 1);
        m_Shader->SetUniform1i("shadowMap", 2);

    }
    

    /**
     * @brief Reinitializes the test model in case of window resize.
     * 
     * @param width New width.
     * @param height New height.
     */
    void CarModels::ModelReinit(int width, int height) 
    {
        WIDTH = width;
        HEIGHT = height;
        modelInit();
    }

    
    CarModels::~CarModels() 
    {

    }

    
    void CarModels::OnUpdate(float deltaTime) 
    {
        
    }

    
    /**
     * @brief Does the drawing.
     * 
     */
    void CarModels::OnRender() 
    {
        glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_Camera->Input(m_Window);
        m_Camera->updateMatrix(45.0f, 0.1f, 100.0f);

        glm::mat4 view = m_Camera->GetView();
        glm::vec3 viewPos = m_Camera->GetPosition();
        glm::mat4 projection = m_Camera->GetProjection();
        glm::mat4 camMetrix = m_Camera->GetMatrix();
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 40.0f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        
        // Creating depth texture
        //-------------------
        if (shadows)
        {
            m_DepthShader->Bind();
            m_DepthShader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);

            glViewport(0, 0, 1024, 1024);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            m_PlaneModel->Draw(*m_DepthShader, *m_Camera);

            if (renderObjects)
            {
                m_ObjectModel->Draw(*m_DepthShader, *m_Camera);
            }
            
            m_Model->Draw(*m_DepthShader, *m_Camera);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glViewport(0, 0, WIDTH, HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        //-------------------

        // Dynamic Cubemap stuff
        if (dynamicReflections)
        {
            
        
            glBindFramebuffer(GL_FRAMEBUFFER, fboID);
            glViewport(0, 0, 2048, 2048);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            for (int side = 0; side < 6; side++){
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, textureID, 0);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glm::mat4 v = glm::mat4(1.0f);
                glm::mat4 p = glm::mat4(1.0f);
                p = glm::perspective(glm::radians((float)90), (float)1, (float)0.1, (float)1000);
                
                switch (GL_TEXTURE_CUBE_MAP_POSITIVE_X + side) {
                    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
                        v = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
                        break;
                    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
                        v = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
                        break;
                    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
                        v = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                        break;
                    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
                        v = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
                        break;
                    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
                        v = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
                        break;
                    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
                        v = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
                        break;
                    default:
                        break;
                }

                // Draw the skybox
                m_SkyboxShader->Bind();
                m_SkyboxShader->SetUniformMat4f("u_View", v);
                m_SkyboxShader->SetUniformMat4f("u_Projection", p);
                m_SkyboxShader->SetUniformMat4f("u_CameraMatrix", p * v);
                m_SkyboxMap->Draw(*m_SkyboxShader, *m_Camera);
                
                // Draw the plane but not the main model
                m_Shader->Bind();
                m_Shader->SetUniform1f("u_LightStrength", lightStrength);
                m_Shader->SetUniform3f("u_LightColor", lightColor.r, lightColor.g, lightColor.b);
                m_Shader->SetUniform3f("u_ViewPos", viewPos.r, viewPos.g, viewPos.b);
                m_Shader->SetUniform3f("u_LightPos", lightPos.x, lightPos.y, lightPos.z);
                m_Shader->SetUniformMat4f("u_CameraMatrix", p * v);
                m_Shader->SetUniform1i("u_ReflectionsBool", int(reflections));
                m_Shader->SetUniform1i("u_ShadowsBool", int(shadows));

                m_Shader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, depthMap);

                if (renderObjects)
                {
                    m_ObjectModel->Draw(*m_Shader, *m_Camera);
                }
                m_PlaneModel->Draw(*m_Shader, *m_Camera);

                m_Shader->Unbind();

                // Render light cube
                m_LightCubeShader->Bind();
                m_LightCubeShader->SetUniformMat4f("u_CameraMatrix", p * v);
                m_LightCube->Draw(*m_LightCubeShader, *m_Camera, glm::vec3(1.0, 1.0, 1.0), lightPos);

            }
        }
        //-------------------
        
        // Render the scene
        //-------------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, WIDTH, HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_SkyboxShader->Bind();
        m_SkyboxShader->SetUniformMat4f("u_View", glm::mat4(glm::mat3(view)));
        m_SkyboxShader->SetUniformMat4f("u_Projection", projection);
        m_SkyboxShader->SetUniformMat4f("u_CameraMatrix", camMetrix);
        m_SkyboxMap->Draw(*m_SkyboxShader, *m_Camera);

        
        m_Shader->Bind();
        m_Shader->SetUniform1f("u_LightStrength", lightStrength);
        m_Shader->SetUniform3f("u_LightColor", lightColor.r, lightColor.g, lightColor.b);
        m_Shader->SetUniform3f("u_ViewPos", viewPos.r, viewPos.g, viewPos.b);
        m_Shader->SetUniform3f("u_LightPos", lightPos.x, lightPos.y, lightPos.z);
        m_Shader->SetUniform1i("u_ReflectionsBool", int(reflections));
        m_Shader->SetUniform1i("u_ShadowsBool", int(shadows));
        m_Camera->Matrix(*m_Shader, "u_CameraMatrix");

        m_Shader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        m_Shader->SetUniform1i("u_ReflectionsBool", 0);
        m_PlaneModel->Draw(*m_Shader, *m_Camera);
        if (renderObjects)
        {
            m_ObjectModel->Draw(*m_Shader, *m_Camera);
        }
        
        m_Shader->SetUniform1i("u_ReflectionsBool", reflections);
        
        if (dynamicReflections)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        }
        else
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxMap->GetTextureID());
        }
        
    
        m_Model->Draw(*m_Shader, *m_Camera, glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, -0.5, 0.0));
        //------

        // Render light cube
        m_LightCubeShader->Bind();
        m_Camera->Matrix(*m_LightCubeShader, "u_CameraMatrix");
        m_LightCube->Draw(*m_LightCubeShader, *m_Camera, glm::vec3(1.0, 1.0, 1.0), lightPos);

    }
    

    /**
     * @brief Creates GUI elements.
     * 
     */
    void CarModels::OnImGuiRender() 
    {
        if (ImGui::CollapsingHeader("Light")){
            ImGui::Indent();
		    ImGui::SliderFloat("Intensity", &lightStrength, 0, 20);
            ImGui::SliderFloat3("Light Position", &lightPos.x, -30, 30);
            ImGui::ColorEdit3("Color", &lightColor.r);
            ImGui::Unindent();
        }
        

        
        if (ImGui::BeginCombo("##combo", currentSkybox.c_str()))
        {
            for (int n = 0; n < skyboxesVec.size(); n++)
            {
                bool is_selected = (currentSkybox == skyboxesVec[n]);
                if (ImGui::Selectable(skyboxesVec[n].c_str(), is_selected))
                    currentSkybox = skyboxesVec[n];

                if (is_selected){
                    
                    ImGui::SetItemDefaultFocus();
                }

                if (currentSkybox != prevSkybox)
                {
                    prevSkybox = currentSkybox;
                    std::vector<std::string> newFaces = getSkyboxFaces(currentSkybox);
                    m_SkyboxMap.reset();

                    m_SkyboxMap = std::make_unique<CubeMap>(newFaces);
                }
                
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("##combo2", currentModel.c_str()))
        {
            for (int n = 0; n < modelsVec.size(); n++)
            {
                bool is_selected = (currentModel == modelsVec[n]);
                if (ImGui::Selectable(modelsVec[n].c_str(), is_selected))
                    currentModel = modelsVec[n];

                if (is_selected){
                    
                    ImGui::SetItemDefaultFocus();
                }

                if (currentModel != prevModel)
                {
                    prevModel = currentModel;
                    m_Model.reset();
                    m_Model = std::make_unique<Model>("res/models/" + currentModel + "/" + currentModel + ".obj", "res/models/" + currentModel + "/");
                }
                
            }
            ImGui::EndCombo();
        }
        
        if (ImGui::BeginCombo("##combo3", currentPlane.c_str()))
        {
            for (int n = 0; n < planesVec.size(); n++)
            {
                bool is_selected = (currentPlane == planesVec[n]);
                if (ImGui::Selectable(planesVec[n].c_str(), is_selected))
                    currentPlane = planesVec[n];

                if (is_selected){
                    
                    ImGui::SetItemDefaultFocus();
                }

                if (currentPlane != prevPlane)
                {
                    prevPlane = currentPlane;
                    m_PlaneModel.reset();
                    m_PlaneModel = std::make_unique<Model>("res/planes/" + currentPlane + "/" + currentPlane + ".obj", "res/planes/" + currentPlane + "/");
                }
                
            }
            ImGui::EndCombo();
        }
        
        ImGui::Checkbox("Render Objects?", &renderObjects);

        if (ImGui::BeginCombo("##combo4", currentObject.c_str()))
        {
            for (int n = 0; n < objectsVec.size(); n++)
            {
                bool is_selected = (currentObject == objectsVec[n]);
                if (ImGui::Selectable(objectsVec[n].c_str(), is_selected))
                    currentObject = objectsVec[n];

                if (is_selected){
                    
                    ImGui::SetItemDefaultFocus();
                }

                if (currentObject != prevObject)
                {
                    prevObject = currentObject;
                    m_ObjectModel.reset();
                    m_ObjectModel = std::make_unique<Model>("res/objects/" + currentObject + "/" + currentObject + ".obj", "res/objects/" + currentObject + "/");
                }
                
            }
            ImGui::EndCombo();
        }

        ImGui::Checkbox("Reflections", &reflections);
        ImGui::Checkbox("Shadows", &shadows);
        ImGui::Checkbox("Dynamic Environment Mapping", &dynamicReflections);
    }
}


/**
 * @brief Search the directory for desired models or cubemaps.
 * 
 * @param path 
 * @param remove 
 * @return std::vector<std::string> 
 */
std::vector<std::string> searchDirectory(std::string path, int remove = 0)
{
    /*
        Inspired by:
        https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
    */
    std::vector<string> vec;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            std::string file(ent->d_name);
            if (file == ".." || file == "." || file == "readme.txt" || file == "other")
            {
                continue;
            }
            file.erase(file.length() - remove);
            vec.push_back(file);
        }
        closedir (dir);
    } else {
        perror ("");
        return vec;
    }

    return vec;
}