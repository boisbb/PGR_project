#pragma once

#include "TestModel.h"
#include "../VertexBuffer.h"
#include "../VertexBufferLayout.h"
#include "../Texture.h"
#include "../Camera.h"
#include "../Model.h"
#include "../Debug.h"
#include "../CubeMap.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>



namespace test_model {
    class ShadowMapping : public TestModel{
    public:
        ShadowMapping();
        ShadowMapping(GLFWwindow* window, int width, int height);
        ~ShadowMapping();

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
        std::unique_ptr<Model> m_ShadowMappingModel;
        std::unique_ptr<Model> m_LightCube;
        std::unique_ptr<Model> m_PlaneModel;
        std::unique_ptr<CubeMap> m_SkyboxMap;

        glm::vec3 objectPos;
        glm::mat4 objectModelInit = glm::mat4(1.0f);
        glm::mat4 objectModel = glm::mat4(1.0f);

        float lightStrength = 0.1f;
        glm::vec3 lightPos = glm::vec3(-2.0f,4.0f,-1.0f);
        glm::vec3 lightColor = glm::vec3(14.0f, 8.0f, -1.0f);

        int WIDTH;
        int HEIGHT;

        unsigned int depthMapFBO;
        unsigned int depthMap;
        unsigned int planeVBO;
        unsigned int woodTexture;
        
    };
}