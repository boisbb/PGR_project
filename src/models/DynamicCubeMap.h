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
    class DynamicCubeMap : public TestModel{
    public:
        DynamicCubeMap();
        DynamicCubeMap(GLFWwindow* window, int width, int height);
        ~DynamicCubeMap();

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
        std::unique_ptr<Shader> m_DebugShader;
        std::unique_ptr<Shader> m_SkyboxShader;

        std::unique_ptr<Camera> m_Camera;
        std::unique_ptr<Texture> m_Texture;
        std::unique_ptr<Model> m_DynamicCubeMapModel;
        std::unique_ptr<Model> m_LightCube;
        std::unique_ptr<Model> m_PlaneModel;
        std::unique_ptr<CubeMap> m_SkyboxMap;

        glm::vec3 objectPos;
        glm::mat4 objectModelInit = glm::mat4(1.0f);
        glm::mat4 objectModel = glm::mat4(1.0f);

        float lightStrength = 0.7f;
        glm::vec3 lightPos = glm::vec3(-3.8f,-2.3f,2.2f);
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

        int WIDTH;
        int HEIGHT;

        //-- DCM stuff
        unsigned int fboID;
        unsigned int renderID;
        unsigned int textureID;

    };
}