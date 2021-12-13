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
    class CarModels : public TestModel{
    public:
        CarModels();
        CarModels(GLFWwindow* window, int width, int height);
        ~CarModels();

        void modelInit();
        void ModelReinit(int width, int height) override;
        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
    private: 
        float m_ClearColor[4];
        GLFWwindow* m_Window;

        std::unique_ptr<Shader> m_Shader;
        std::unique_ptr<Shader> m_LightCubeShader;
        std::unique_ptr<Shader> m_DepthShader;
        std::unique_ptr<Shader> m_DebugShader;
        std::unique_ptr<Shader> m_SkyboxShader;

        std::unique_ptr<Camera> m_Camera;
        std::unique_ptr<Texture> m_Texture;
        std::unique_ptr<Model> m_Model;
        std::unique_ptr<Model> m_LightCube;
        std::unique_ptr<Model> m_PlaneModel;
        std::unique_ptr<CubeMap> m_SkyboxMap;

        unsigned int depthMapFBO;
        unsigned int depthMap;

        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

        float lightStrength = 0.5f;
        glm::vec3 lightPos = glm::vec3(-10.2f,18.0f,-9.43f);
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

        std::vector<std::string> skyboxesVec;
        std::string currentSkybox = "mountain";
        std::string prevSkybox = "mountain";

        std::vector<std::string> modelsVec;
        std::string currentModel = "porsche";
        std::string prevModel = "porsche";

        bool reflections = true;
        bool shadows = true;

        int WIDTH;
        int HEIGHT;


        
    };
}