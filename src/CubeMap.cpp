#include "CubeMap.h"

#include "vendor/stb_image/stb_image.h"
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"


CubeMap::CubeMap(std::vector<std::string> faces){
    std::vector<glm::vec3> skyboxVertices = {
        // positions          
        glm::vec3{-1.0f,  1.0f, -1.0f},
        glm::vec3{-1.0f, -1.0f, -1.0f},
        glm::vec3{ 1.0f, -1.0f, -1.0f},
        glm::vec3{ 1.0f, -1.0f, -1.0f},
        glm::vec3{ 1.0f,  1.0f, -1.0f},
        glm::vec3{-1.0f,  1.0f, -1.0f},

        glm::vec3{-1.0f, -1.0f,  1.0f},
        glm::vec3{-1.0f, -1.0f, -1.0f},
        glm::vec3{-1.0f,  1.0f, -1.0f},
        glm::vec3{-1.0f,  1.0f, -1.0f},
        glm::vec3{-1.0f,  1.0f,  1.0f},
        glm::vec3{-1.0f, -1.0f,  1.0f},

        glm::vec3{ 1.0f, -1.0f, -1.0f},
        glm::vec3{ 1.0f, -1.0f,  1.0f},
        glm::vec3{ 1.0f,  1.0f,  1.0f},
        glm::vec3{ 1.0f,  1.0f,  1.0f},
        glm::vec3{ 1.0f,  1.0f, -1.0f},
        glm::vec3{ 1.0f, -1.0f, -1.0f},

        glm::vec3{-1.0f, -1.0f,  1.0f},
        glm::vec3{-1.0f,  1.0f,  1.0f},
        glm::vec3{ 1.0f,  1.0f,  1.0f},
        glm::vec3{ 1.0f,  1.0f,  1.0f},
        glm::vec3{ 1.0f, -1.0f,  1.0f},
        glm::vec3{-1.0f, -1.0f,  1.0f},

        glm::vec3{-1.0f,  1.0f, -1.0f},
        glm::vec3{ 1.0f,  1.0f, -1.0f},
        glm::vec3{ 1.0f,  1.0f,  1.0f},
        glm::vec3{ 1.0f,  1.0f,  1.0f},
        glm::vec3{-1.0f,  1.0f,  1.0f},
        glm::vec3{-1.0f,  1.0f, -1.0f},

        glm::vec3{-1.0f, -1.0f, -1.0f},
        glm::vec3{-1.0f, -1.0f,  1.0f},
        glm::vec3{ 1.0f, -1.0f, -1.0f},
        glm::vec3{ 1.0f, -1.0f, -1.0f},
        glm::vec3{-1.0f, -1.0f,  1.0f},
        glm::vec3{ 1.0f, -1.0f,  1.0f}
    };

    m_VAO = new VertexArray();
    m_VAO->Bind();
    m_VertexBuffer = new VertexBuffer(skyboxVertices);

    VertexBufferLayout layout;
    layout.Push<float>(3);
    m_VAO->AddBuffer(*m_VertexBuffer, layout);

    skyboxTexture = loadCubemap(faces);

}

void CubeMap::Draw(Shader& shader, Camera& camera){
    glDepthFunc(GL_LEQUAL);
    shader.Bind();

    glm::mat4 objectModelInit = glm::mat4(1.0f);
    objectModelInit = glm::scale(objectModelInit, glm::vec3(200,200,200));

    glm::mat4 view = camera.GetView(); // remove translation from the view matrix
    glm::mat4 projection = camera.GetProjection();
    shader.SetUniformMat4f("u_View", view);
    shader.SetUniformMat4f("u_Projection", projection);
    shader.SetUniformMat4f("u_Scale", objectModelInit);
    // skybox cube
    m_VAO->Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    m_VAO->Unbind();

    glDepthFunc(GL_LESS);
}

CubeMap::~CubeMap(){

}

unsigned int CubeMap::loadCubemap(vector<std::string> faces){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    stbi_set_flip_vertically_on_load(0);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}