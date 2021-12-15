#include "DCMSM.h"
#include "../Debug.h"

#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/imgui/imgui.h"

namespace test_model
{
    DCMSM::DCMSM() 
        : m_ClearColor {0.2f, 0.3f, 0.8f, 1.0f}
    {
    }

    DCMSM::DCMSM(GLFWwindow* window, int width, int height) 
        : m_Window(window), WIDTH(width), HEIGHT(height)
    {
        modelInit();
    }

    void DCMSM::modelInit()
    {
        m_DCMSMModel = std::make_unique<Model>("res/models/porsche/porsche.obj", "res/models/porsche/");
        m_PlaneModel = std::make_unique<Model>("res/models/road/road.obj", "res/models/road/");

        glEnable(GL_DEPTH_TEST);
        m_Camera = std::make_unique<Camera>(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));


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

        //------ Dynamic cubemap stuff
        //Create cubemap textures	//Generate FBO
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

        //Create uniform depth buffer for z-test purposes
        glGenRenderbuffers(1, &renderID);
        glBindRenderbuffer(GL_RENDERBUFFER, renderID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 2048, 2048);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        //attach FBO and -X texture
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, textureID, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		    cerr << "FrameBuffer isn't ok" << endl;
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //--------------------------

        //-- Depthmapping stuff
        m_DepthShader = std::make_unique<Shader>("res/shaders/depth.shader");

        const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
        glGenFramebuffers(1, &depthMapFBO);
        // create depth texture
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //-------------------------

        m_Shader->Bind();
        m_Shader->SetUniform1i("skybox", 0);
        m_Shader->SetUniform1i("u_Texture", 1);
        m_Shader->SetUniform1i("shadowMap", 2);
    }
    
    DCMSM::~DCMSM() 
    {
    }

    void DCMSM::ModelReinit(int width, int height) 
    {
        WIDTH = width;
        HEIGHT = height;
        modelInit();
    }
    
    void DCMSM::OnUpdate(float deltaTime) 
    {
        
    }
    
    void DCMSM::OnRender() 
    {
        GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

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
        m_DepthShader->Bind();
        m_DepthShader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        m_PlaneModel->Draw(*m_DepthShader, *m_Camera);
        m_DCMSMModel->Draw(*m_DepthShader, *m_Camera);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, WIDTH, HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Dynamic CubeMap Stuff
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);
        glViewport(0, 0, 2048, 2048);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (int side = 0; side < 6; side++){
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, textureID, 0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glm::mat4 v = glm::mat4(1.0f);
            glm::mat4 p = glm::mat4(1.0f);
            p = glm::perspective(glm::radians((float)90), (float)1, (float)0.1, (float)1000);
            
            // CHYBA JE V TOM, ZE SE VZDY PREKRESLI POSLEDNI TILE, JAKOBY SE NEMENILA POZICE KAM SE MA VYKRESLOVAT
            //up vectors are flipped to get correct image orientation
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

            m_Shader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            m_PlaneModel->Draw(*m_Shader, *m_Camera);

            m_Shader->Unbind();

            // Render light cube
            m_LightCubeShader->Bind();
            m_LightCubeShader->SetUniformMat4f("u_CameraMatrix", p * v);
            m_LightCube->Draw(*m_LightCubeShader, *m_Camera, glm::vec3(1.0, 1.0, 1.0), lightPos);

            


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
        m_Camera->Matrix(*m_Shader, "u_CameraMatrix");

        m_Shader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        m_PlaneModel->Draw(*m_Shader, *m_Camera);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        m_DCMSMModel->Draw(*m_Shader, *m_Camera, glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, -0.5, 0.0));


        
        //------

        // Render light cube
        m_LightCubeShader->Bind();
        m_Camera->Matrix(*m_LightCubeShader, "u_CameraMatrix");
        m_LightCube->Draw(*m_LightCubeShader, *m_Camera, glm::vec3(1.0, 1.0, 1.0), lightPos);


        ///----
    }
    
    void DCMSM::OnImGuiRender() 
    {
        if (ImGui::CollapsingHeader("Light")){
            ImGui::Indent();
		    ImGui::SliderFloat("Intensity", &lightStrength, 0, 1);
            ImGui::SliderFloat3("Light Position", &lightPos.x, -30, 30);
            ImGui::ColorEdit3("Color", &lightColor.r);
        }
        ImGui::Unindent();
        if (ImGui::CollapsingHeader("Model")){
            ImGui::Indent();
            if (ImGui::SliderFloat3("Model Position", &objectPos.x, -30, 30)){
                objectModel = glm::translate(objectModelInit, objectPos);
                //objectModel = glm::translate(objectModel, objectPos);
                //objectModel = glm::translate(objectModel, glm::vec3(0.0f, 0.0f, 0.0f));
            }

        }

        ImGui::Unindent();
        ImGui::ColorEdit4("Clear Color", m_ClearColor);
    }
}
