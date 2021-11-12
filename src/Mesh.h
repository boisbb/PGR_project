#pragma once

#include <string>
#include <vector>
#include <memory>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Camera.h"
#include "Texture.h"
#include "Renderer.h"

class Mesh
{
public:
    Mesh(){}
    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    ~Mesh();

    void AddTexture(Texture& newTexture);

    void Draw(Shader& shader, Camera& camera, bool setUniformMatrices = false,
              glm::mat4 matrix = glm::mat4(1.0f), glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
              glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<Texture> m_Textures;
    Texture txt;

    VertexArray* m_VAO;
    IndexBuffer* m_IndexBuffer;
    VertexBuffer* m_VertexBuffer;

};