#pragma once

#include <string>
#include <vector>
#include <memory>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Camera.h"
#include "Texture.h"
#include "Renderer.h"

class CubeMap
{
public:
    CubeMap();
    ~CubeMap();
    void Draw(Shader& shader, Camera& camera);

private:
    unsigned int loadCubemap(vector<std::string> faces);
    
    unsigned int skyboxVBO, skyboxVAO, skyboxTexture;

    VertexArray* m_VAO;
    IndexBuffer* m_IndexBuffer;
    VertexBuffer* m_VertexBuffer;

};