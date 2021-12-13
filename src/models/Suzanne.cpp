#include "Suzanne.h"
#include "../Debug.h"

#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/imgui/imgui.h"

namespace test_model
{
    Suzanne::Suzanne() 
        : m_ClearColor {0.2f, 0.3f, 0.8f, 1.0f}
    {
    }

    Suzanne::Suzanne(GLFWwindow* window, int width, int height) 
        : m_Window(window), WIDTH(width), HEIGHT(height)
    {
        modelInit();
    }

    void Suzanne::modelInit()
    {

        m_SuzanneModel = std::make_unique<Model>("res/models/suzanne/suzanne.obj");
        m_PlaneModel = std::make_unique<Model>("res/models/plane/plane.obj");

        glEnable(GL_DEPTH_TEST);
        m_Camera = std::make_unique<Camera>(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));


        m_LightCubeShader = std::make_unique<Shader>("res/shaders/lightCube.shader");
        m_LightCube = std::make_unique<Model>("res/models/lightCube/lightCube.obj");

        m_Shader = std::make_unique<Shader>("res/shaders/shadowmap_mine.shader");
        m_DepthShader = std::make_unique<Shader>("res/shaders/depth.shader");

        m_Texture = std::make_unique<Texture>("res/models/suzanne2/wood.png", 0);
        woodTexture = m_Texture->GetID();



        //--------------------------

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

        m_Shader->Bind();
        m_Shader->SetUniform1i("u_Texture", 1);
        m_Shader->SetUniform1i("shadowMap", 0);

        //--------------------------
    }
    
    Suzanne::~Suzanne() 
    {
    }

    void Suzanne::ModelReinit(int width, int height) 
    {
        WIDTH = width;
        HEIGHT = height;
        modelInit();
    }
    
    void Suzanne::OnUpdate(float deltaTime) 
    {
        
    }
    
    void Suzanne::OnRender() 
    {
        GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        m_Camera->Input(m_Window);
        m_Camera->updateMatrix(45.0f, 0.1f, 100.0f);

        glm::mat4 view = m_Camera->GetView();
        glm::vec3 viewPos = m_Camera->GetPosition();
        glm::mat4 projection = m_Camera->GetProjection();

        
        // Creating depth texture
        //-------------------
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 40.0f;

        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        m_DepthShader->Bind();
        m_DepthShader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        m_PlaneModel->Draw(*m_DepthShader, *m_Camera);
        m_SuzanneModel->Draw(*m_DepthShader, *m_Camera);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, WIDTH, HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render the scene
        //-------------------
        glViewport(0, 0, WIDTH, HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_Shader->Bind();

        
        m_Shader->SetUniform1f("u_LightStrength", lightStrength);
        m_Shader->SetUniform3f("u_LightColor", lightColor.r, lightColor.g, lightColor.b);
        m_Shader->SetUniform3f("u_ViewPos", viewPos.r, viewPos.g, viewPos.b);
        m_Shader->SetUniform3f("u_LightPos", lightPos.x, lightPos.y, lightPos.z);
        m_Shader->SetUniform1i("u_HasTexture", 1);
        m_Camera->Matrix(*m_Shader, "u_CameraMatrix");

        m_Shader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        m_PlaneModel->Draw(*m_Shader, *m_Camera);
        m_SuzanneModel->Draw(*m_Shader, *m_Camera);
        //------

        // Render light cube
        m_LightCubeShader->Bind();
        m_Camera->Matrix(*m_LightCubeShader, "u_CameraMatrix");
        m_LightCube->Draw(*m_LightCubeShader, *m_Camera, glm::vec3(1.0, 1.0, 1.0), lightPos);
        
    }
    
    void Suzanne::OnImGuiRender() 
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
