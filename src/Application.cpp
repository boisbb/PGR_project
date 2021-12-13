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
#include "models/ShadowMapping.h"
#include "models/TexturedCube.h"
#include "models/CarModels.h"

int WIDTH = 1200;
int HEIGHT = 800;


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
    window = glfwCreateWindow(WIDTH, HEIGHT, "PGR Project", NULL, NULL);
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

    testModelMenu->RegisterTestModel<test_model::Suzanne>("Suzanne", window, WIDTH, HEIGHT);
    testModelMenu->RegisterTestModel<test_model::ShadowMapping>("ShadowMapping", window, WIDTH, HEIGHT);
    testModelMenu->RegisterTestModel<test_model::TexturedCube>("TexturedCube", window, WIDTH, HEIGHT);
    testModelMenu->RegisterTestModel<test_model::CarModels>("CarModels", window, WIDTH, HEIGHT);
    if (argc == 2)
    {
        currentTestModel = testModelMenu->SetTestModel(argv[1]);
        
    }

    while (!glfwWindowShouldClose(window))
    {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        renderer.Clear();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        if (w != WIDTH || h != HEIGHT)
        {
            WIDTH = w;
            HEIGHT = h;
            GLCall(glViewport(0,0, w, h));
            currentTestModel->ModelReinit(w, h);
        }
        

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
