#pragma once

#include "TestModel.h"
#include "../VertexBuffer.h"
#include "../VertexBufferLayout.h"
#include "../Texture.h"
#include "../Camera.h"
#include "../Model.h"
#include "../CubeMap.h"
#include "../Debug.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>



namespace test_model {
    class ChevyCamaro : public TestModel{
    public:
        ChevyCamaro();
        ChevyCamaro(GLFWwindow* window);
        ~ChevyCamaro();

        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
    private: 
        float m_ClearColor[4];
        GLFWwindow* m_Window;

        std::unique_ptr<Shader> m_Shader;
        std::unique_ptr<Shader> m_SkyboxShader;
        std::unique_ptr<Camera> m_Camera;
        std::unique_ptr<Texture> m_Texture;
        std::unique_ptr<Model> m_Model;
        std::unique_ptr<CubeMap> m_SkyboxMap;

        glm::vec3 objectPos;
        glm::mat4 objectModelInit = glm::mat4(1.0f);
        glm::mat4 objectModel = glm::mat4(1.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

        float lightStrength = 0.8f;
        glm::vec3 lightPos = glm::vec3(4,4,4);
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);


        
    };
}