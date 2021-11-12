#include "Camera.h"

#include <string>
#include <iostream>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"

Camera::Camera(int width, int height, glm::vec3 position)
    : c_Width(width), c_Height(height), c_Position(position)
{
    
}

Camera::~Camera() 
{
    
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane) 
{
    glm::mat4 c_View = glm::mat4(1.0f);
    glm::mat4 c_Projection = glm::mat4(1.0f);

    c_View = glm::lookAt(c_Position, c_Position + c_Orientation, c_Up);
    c_Projection = glm::perspective(glm::radians(FOVdeg), (float)(c_Width / c_Height), nearPlane, farPlane);

    
    c_Matrix = c_Projection * c_View;
}

void Camera::Input(GLFWwindow* window) 
{   
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
	    return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        c_Position += c_Speed * c_Orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        c_Position += c_Speed * -glm::normalize(glm::cross(c_Orientation, c_Up));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        c_Position += c_Speed * -c_Orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        c_Position += c_Speed * glm::normalize(glm::cross(c_Orientation, c_Up));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        c_Position += c_Speed * c_Up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
        c_Position += c_Speed * -c_Up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        c_Speed = 0.4f;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE){
        c_Speed = 0.1f;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        if (firstClick)
        {
            glfwSetCursorPos(window, (c_Width / 2), (c_Height / 2));
            firstClick = false;
        }
        

        double mouseX;
        double mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float rotx = sensitivity * (float)(mouseY - (c_Height / 2)) / c_Height;
        float roty = sensitivity * (float)(mouseX - (c_Height / 2)) / c_Height;
        glm::vec3 newOrientation = glm::rotate(c_Orientation, glm::radians(-rotx), glm::normalize(glm::cross(c_Orientation, c_Up)));

        if (!((glm::angle(newOrientation, c_Up) <= glm::radians(0.5f)) || (glm::angle(newOrientation, -c_Up) <= glm::radians(5.0f))))
        {
            c_Orientation = newOrientation;
        }

        c_Orientation = glm::rotate(c_Orientation, glm::radians(-roty), c_Up);

        glfwSetCursorPos(window, (c_Width / 2), (c_Height / 2));
        
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstClick = true;
    }
}

void Camera::Matrix(Shader& shader, const char* uniform) 
{
    shader.SetUniformMat4f(uniform, c_Matrix);
}

glm::vec3 Camera::GetPosition() const
{
    return c_Position;
}
