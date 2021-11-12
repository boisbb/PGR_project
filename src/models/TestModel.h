#pragma once

#include <functional>
#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace test_model {
    
    class TestModel{
    public:
        TestModel() {}
        virtual ~TestModel() {}

        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender() {}
        virtual void OnImGuiRender() {}
    };

    class TestModelMenu : public TestModel{
    public:
        TestModelMenu(TestModel*& currentTestModelPointer);

        void OnImGuiRender() override;
        TestModel* SetTestModel(const char* testModelName);

        template<typename T>
        void RegisterTestModel(const std::string& name){
            std::cout << "Registering test_model " << name << std::endl;

            m_TestModels.push_back(std::make_pair(name, []() { return new T(); }));
        }

        template<typename T>
        void RegisterTestModel(const std::string& name, GLFWwindow* window){
            std::cout << "Registering test_model " << name << std::endl;

            m_TestModels.push_back(std::make_pair(name, [window]() { return new T(window); }));
        }
        
    private:
        TestModel*& m_CurrentTestModel;
        std::vector<std::pair<std::string, std::function<TestModel*()>>> m_TestModels;
    };
}