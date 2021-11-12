#include <GL/glew.h>
#include <GLFW/glfw3.h>

/*
http://docs.gl/
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Debug.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"

#include "internal/model_loader.h"

#include "models/TestModel.h"
#include "models/Suzanne.h"

#define WIDTH 800
#define HEIGHT 800


int main(int argc, char* argv[])
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        cout << "Error glewInit()" << endl;
        return 1;
    }
    cout << glGetString(GL_VERSION) << endl;

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glViewport(0,0, WIDTH, HEIGHT));

    Renderer renderer;

    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();

    test_model::TestModel* currentTestModel = NULL;
    test_model::TestModelMenu* testModelMenu = new test_model::TestModelMenu(currentTestModel);
    currentTestModel = testModelMenu;

    testModelMenu->RegisterTestModel<test_model::Suzanne>("Suzanne", window);

    if (argc == 2)
    {
        currentTestModel = testModelMenu->SetTestModel(argv[1]);
        
    }
    /*

    Shader m_Shader("res/shaders/lighting.shader");
    Camera m_Camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));
    Texture m_Texture("res/textures/uvmap_suzanne.png", 0);
    
    Model cubeModel("res/models/suzanne.obj");
    cubeModel.AddMeshTexture(m_Texture, 0);

    glm::vec3 objectPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 objectModel = glm::mat4(1.0f);
    objectModel = glm::translate(objectModel, objectPos);

    m_Shader.Bind();
    m_Shader.SetUniformMat4f("u_ModelMatrix", objectModel);
    //m_Shader.SetUniform4f("u_LightColor", lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    //m_Shader.SetUniform3f("u_LightPosition", lightPos.x, lightPos.y, lightPos.z);
    */


    while (!glfwWindowShouldClose(window))
    {
        GLCall(glClearColor(0.07f, 0.13f, 0.17f, 1.0f));
        renderer.Clear();

        ImGui_ImplGlfwGL3_NewFrame();
        ImGui::Begin("Menu");
        if(currentTestModel){
            currentTestModel->OnUpdate(0.0f);
            currentTestModel->OnRender();

            if (currentTestModel != testModelMenu && (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS || ImGui::Button("<-"))){
                delete currentTestModel;
                currentTestModel = testModelMenu;
            }
            currentTestModel->OnImGuiRender();
        }
        ImGui::End();
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());



        /*
        GLCall(glClearColor(0.07f, 0.13f, 0.17f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        m_Camera.Input(window);
        m_Camera.updateMatrix(45.0f, 0.1f, 100.0f);
        
        m_Shader.Bind();

        //glm::vec3 camPos = m_Camera.GetPosition();
        //m_Shader.SetUniform3f("u_CameraPosition", camPos.x, camPos.y, camPos.z);

        // Setting transformation matrices
        glm::mat4 view = m_Camera.GetView();
        glm::vec3 lightPos = glm::vec3(4,4,4);
        glm::vec3 viewPos = m_Camera.GetPosition();

        
        //m_Shader.SetUniformMat4f("u_View", view);
        m_Shader.SetUniform3f("u_ViewPos", viewPos.x, viewPos.y, viewPos.z);
        m_Shader.SetUniformMat4f("u_ModelMatrix", objectModel);
        m_Shader.SetUniform3f("u_LightPos", lightPos.x, lightPos.y, lightPos.z);
        m_Camera.Matrix(m_Shader, "u_CameraMatrix");
        
        cubeModel.Draw(m_Shader, m_Camera);
        */
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    delete currentTestModel;
    if (currentTestModel != testModelMenu){
        delete testModelMenu;
    }
    
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
