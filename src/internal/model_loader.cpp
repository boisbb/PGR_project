#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

#include "../vendor/glm/glm.hpp"
#include "model_loader.h"


bool loadOBJ(const char* path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals) 
{
    return false;
}

bool loadAssImpMeshes(const char* path, std::vector<Mesh>& meshes)
{

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, 0);

    if (!scene)
    {
        fprintf(stderr, importer.GetErrorString());
        getchar();
        return false;
    }

    meshes.reserve(scene->mNumMeshes);
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        std::vector<unsigned int> indices;
        std::vector<Vertex> vertices;

        const aiMesh* mesh = scene->mMeshes[i];

        vertices.reserve(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            aiVector3D pos = mesh->mVertices[i];
            //vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));

            aiVector3D UVW = mesh->mTextureCoords[0][i];
            //uvs.push_back(glm::vec2(UVW.x, UVW.y));

            aiVector3D n = mesh->mNormals[i];
            //normals.push_back(glm::vec3(n.x, n.y, n.z));

            vertices.push_back(Vertex{glm::vec3(pos.x, pos.y, pos.z), glm::vec2(UVW.x, UVW.y), glm::vec3(n.x, n.y, n.z)});
        }

        indices.reserve(3*mesh->mNumFaces);
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            indices.push_back(mesh->mFaces[i].mIndices[0]);
            indices.push_back(mesh->mFaces[i].mIndices[1]);
            indices.push_back(mesh->mFaces[i].mIndices[2]);
        }

        meshes.push_back(Mesh(vertices, indices));


    }
    

    return true;
}

bool loadAssImpVert(const char* path, std::vector<unsigned int>& indices, std::vector<Vertex>& vertices)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, 0);

    if (!scene)
    {
        fprintf(stderr, importer.GetErrorString());
        getchar();
        return false;
    }

    const aiMesh* mesh = scene->mMeshes[0];

    vertices.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        aiVector3D pos = mesh->mVertices[i];
        //vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));

        aiVector3D UVW = mesh->mTextureCoords[0][i];
        //uvs.push_back(glm::vec2(UVW.x, UVW.y));

        aiVector3D n = mesh->mNormals[i];
        //normals.push_back(glm::vec3(n.x, n.y, n.z));

        vertices.push_back(Vertex{glm::vec3(pos.x, pos.y, pos.z), glm::vec2(UVW.x, UVW.y), glm::vec3(n.x, n.y, n.z)});
    }

    indices.reserve(3*mesh->mNumFaces);
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        indices.push_back(mesh->mFaces[i].mIndices[0]);
        indices.push_back(mesh->mFaces[i].mIndices[1]);
        indices.push_back(mesh->mFaces[i].mIndices[2]);
    }

    return true;


}

bool loadAssImp(const char* path, std::vector<unsigned int>& indices, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals) 
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, 0);

    if (!scene)
    {
        fprintf(stderr, importer.GetErrorString());
        getchar();
        return false;
    }

    const aiMesh* mesh = scene->mMeshes[0];

    vertices.reserve(mesh->mNumAnimMeshes);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        aiVector3D pos = mesh->mVertices[i];
        vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
    }

    uvs.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        aiVector3D UVW = mesh->mTextureCoords[0][i];
        uvs.push_back(glm::vec2(UVW.x, UVW.y));
    }

    normals.reserve(3*mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        aiVector3D n = mesh->mNormals[i];
        normals.push_back(glm::vec3(n.x, n.y, n.z));
    }

    indices.reserve(3*mesh->mNumFaces);
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        indices.push_back(mesh->mFaces[i].mIndices[0]);
        indices.push_back(mesh->mFaces[i].mIndices[1]);
        indices.push_back(mesh->mFaces[i].mIndices[2]);
    }
    

    return true;
    
    
}
