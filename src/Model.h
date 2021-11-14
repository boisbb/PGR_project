#pragma once

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "vendor/glm/glm.hpp"

#include "Mesh.h"


class Model
{
public:
    Model();
    Model(const char* file);
    ~Model();

    void Draw(Shader& shader, Camera& camera, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
    void AddMeshTexture(Texture& texture, int position);
    void AddMesh(Mesh& mesh);
private:
    const char* file;
    std::vector<Mesh> meshes;

    std::vector<Texture> loadedTex;

    void processMesh(aiMesh *mesh, const aiScene *scene, aiMatrix4x4 accTransform);
    void processNode(aiNode *node, const aiScene *scene, aiMatrix4x4 accTransform);
    bool loadModel(const char* path);
};