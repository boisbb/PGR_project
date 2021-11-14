#include "TexturedCube.h"
#include "../Debug.h"

#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/imgui/imgui.h"

#define WIDTH 800
#define HEIGHT 800

namespace test_model
{
    TexturedCube::TexturedCube() 
        : m_ClearColor {0.2f, 0.3f, 0.8f, 1.0f}
    {
    }

    TexturedCube::TexturedCube(GLFWwindow* window) 
        : m_Window(window)
    {
        m_Shader = std::make_unique<Shader>("res/shaders/lighting.shader");
        m_Camera = std::make_unique<Camera>(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));
        m_Texture = std::make_unique<Texture>("res/textures/uvmap.png", 0);

        m_CubeModel = std::make_unique<Model>("res/models/cube.obj");
        m_CubeModel->AddMeshTexture(*m_Texture, 0);

        objectPos = glm::vec3(0.0f, 0.0f, 0.0f);
        objectModelInit = glm::mat4(1.0f);
        objectModelInit = glm::translate(objectModelInit, objectPos);
        objectModel = objectModelInit;

        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_ModelMatrix", objectModel);
    }
    
    TexturedCube::~TexturedCube() 
    {
    }
    
    void TexturedCube::OnUpdate(float deltaTime) 
    {
        
    }
    
    void TexturedCube::OnRender() 
    {
        GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        m_Camera->Input(m_Window);
        m_Camera->updateMatrix(45.0f, 0.1f, 100.0f);

        m_Shader->Bind();

        glm::mat4 view = m_Camera->GetView();
        glm::vec3 viewPos = m_Camera->GetPosition();

        m_Shader->SetUniform1f("u_LightStrength", lightStrength);
        m_Shader->SetUniform3f("u_LightColor", lightColor.r, lightColor.g, lightColor.b);
        m_Shader->SetUniformMat4f("u_ModelMatrix", objectModel);
        m_Shader->SetUniform3f("u_LightPos", lightPos.x, lightPos.y, lightPos.z);
        m_Shader->SetUniform1i("u_HasTexture", 1);
        m_Camera->Matrix(*m_Shader, "u_CameraMatrix");
        
        m_CubeModel->Draw(*m_Shader, *m_Camera);


    }
    
    void TexturedCube::OnImGuiRender() 
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