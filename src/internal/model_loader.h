#pragma once

#include "../VertexBuffer.h"
#include "../Mesh.h"


bool loadOBJ(const char* path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals);

bool loadAssImpMeshes(const char* path, std::vector<Mesh>& meshes);

bool loadAssImpVert(const char* path, std::vector<unsigned int>& indices, std::vector<Vertex>& vertices);

bool loadAssImp(const char* path, std::vector<unsigned int>& indices, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals);