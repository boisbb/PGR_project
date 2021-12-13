#include "HondaCivic.h"
#include "../Debug.h"

#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/imgui/imgui.h"

#define WIDTH 800
#define HEIGHT 800

namespace test_model
{
    HondaCivic::HondaCivic() 
        : m_ClearColor {0.2f, 0.3f, 0.8f, 1.0f}
    {
    }

    HondaCivic::HondaCivic(GLFWwindow* window) 
        : m_Window(window)
    {
        m_Shader = std::make_unique<Shader>("res/shaders/skyboxobject_notexture.shader");

        m_Camera = std::make_unique<Camera>(WIDTH, HEIGHT, 
                                            //glm::vec3(2.74633, 1.87862, 5.50846),
                                            glm::vec3(0, 0, 0),
                                            glm::vec3(-0.482962, -0.19509, -0.853632),
                                            glm::vec3(0, 1, 0)
                                            );
        //m_Texture = std::make_unique<Texture>("res/models/porsche/Porche.png", 0);
        m_Model = std::make_unique<Model>("res/models/civic/civic.obj", "res/models/porsche/");
        //m_Model->AddMeshTexture(*m_Texture, 23);

        m_SkyboxShader = std::make_unique<Shader>("res/shaders/skybox.shader");
        
        vector<std::string> faces
        {
            "res/textures/skybox/right.jpg",
            "res/textures/skybox/left.jpg",
            "res/textures/skybox/top.jpg",
            "res/textures/skybox/bottom.jpg",
            "res/textures/skybox/front.jpg",
            "res/textures/skybox/back.jpg"
        };

        m_SkyboxMap = std::make_unique<CubeMap>(faces);

        objectPos = glm::vec3(0.0f, 0.0f, 0.0f);
        objectModelInit = glm::mat4(1.0f);
        objectModelInit = glm::translate(objectModelInit, objectPos);
        objectModelInit = glm::scale(objectModelInit, glm::vec3(0.1,0.1,0.1));
        objectModel = objectModelInit;

        scale = glm::vec3(1.0,1.0,1.0);

        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_ModelMatrix", objectModel);
    }
    
    HondaCivic::~HondaCivic() 
    {

    }
    
    void HondaCivic::OnUpdate(float deltaTime) 
    {
        
    }
    
    void HondaCivic::OnRender() 
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        m_Camera->Input(m_Window);
        m_Camera->updateMatrix(45.0f, 0.1f, 100.0f);

        glm::mat4 camMetrix = m_Camera->GetMatrix();
        glm::vec3 viewPos = m_Camera->GetPosition();
        glm::mat4 projection = m_Camera->GetProjection();


        glm::mat4 view = glm::mat4(glm::mat3(m_Camera->GetView()));
        m_SkyboxShader->Bind();
        m_SkyboxShader->SetUniformMat4f("u_View", view);
        m_SkyboxShader->SetUniformMat4f("u_Projection", projection);
        m_SkyboxShader->SetUniformMat4f("u_CameraMatrix", camMetrix);
        m_SkyboxMap->Draw(*m_SkyboxShader, *m_Camera);

        m_Shader->Bind();

        m_Shader->SetUniform1f("u_LightStrength", lightStrength);
        m_Shader->SetUniform3f("u_LightColor", lightColor.r, lightColor.g, lightColor.b);
        m_Shader->SetUniform3f("u_ViewPos", viewPos.r, viewPos.g, viewPos.b);
        m_Shader->SetUniform3f("u_LightPos", lightPos.x, lightPos.y, lightPos.z);
        m_Shader->SetUniformMat4f("u_CameraMatrix", camMetrix);
        m_Shader->SetUniform3f("u_CameraPos", viewPos.x, viewPos.y, viewPos.z);
        m_Model->Draw(*m_Shader, *m_Camera, scale);

    }
    
    void HondaCivic::OnImGuiRender() 
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

// kresleni transparentnich objektu 2x
// 1. nakreslim odvracene - glEnable(GL_CULL_FACE)
// 2. nakreslim privracene - zahazovani trojuhelniku, ktere jsou odvracene
// fragment shader - discard - dithering
// a-buffer / order independent transparency