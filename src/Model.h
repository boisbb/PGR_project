#pragma once

#include "vendor/nlohmann/json/json.hpp"
#include "internal/model_loader.h"

#include "Mesh.h"

using json = nlohmann::json;

std::string get_file_contents(const char* filename);

class Model
{
public:
    Model();
    Model(const char* file);
    ~Model();

    void Draw(Shader& shader, Camera& camera);
    void AddMeshTexture(Texture& texture, int position);
    void AddMesh(Mesh& mesh);
private:
    const char* file;
    std::vector<Mesh> meshes;

    std::vector<Texture> loadedTex;
};