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

Mesh processMesh(aiMesh *mesh, const aiScene *scene)
{

    std::vector<unsigned int> indices;
    std::vector<Vertex> vertices;

    aiVector3D UVW;
    aiVector3D n;

    // ADD TEXTURE LOADING IF POSSIBLE
    for (unsigned int j = 0; j < mesh->mNumVertices; j++)
    {

        Vertex vertex;

        glm::vec3 vector; 
        vector.x = mesh->mVertices[j].x;
        vector.y = mesh->mVertices[j].y;
        vector.z = mesh->mVertices[j].z; 
        vertex.position = vector;

        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][j].x; 
                vec.y = mesh->mTextureCoords[0][j].y;
                vertex.texUV = vec;
            }
            else
                vertex.texUV = glm::vec2(0.0f, 0.0f);


        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[j].x;
            vector.y = mesh->mNormals[j].y;
            vector.z = mesh->mNormals[j].z;
            vertex.normal = vector;
        }
        
        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    } 

    /*
    // process indices
    [...]
    // process material
    if(mesh->mMaterialIndex >= 0)
    {
        [...]
    }
    */

    return Mesh(vertices, indices);
}

void processNode(aiNode *node, const aiScene *scene, std::vector<Mesh>& meshes)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(processMesh(mesh, scene));			
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, meshes);
    }
}


bool loadAssImpEnhanced(const char* path, std::vector<Mesh>& meshes)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return false;
    }

    processNode(scene->mRootNode, scene, meshes);

    std::cout<<meshes.size()<<std::endl;

    return true;

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

    std::cout<<scene->mNumMeshes<<std::endl;
    
    aiVector3D UVW;
    aiVector3D n;

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        std::vector<unsigned int> indices;
        std::vector<Vertex> vertices;

        const aiMesh* mesh = scene->mMeshes[i];

        // ADD TEXTURE LOADING IF POSSIBLE
        //std::cout<<mesh->mMaterialIndex<<std::endl;
        //std::cout<<(scene->mMaterials[0]->GetName().C_Str())<<std::endl;


        std::cout<<mesh->mNumVertices<<std::endl;
        vertices.reserve(mesh->mNumVertices);
        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            aiVector3D pos = mesh->mVertices[j];
            //vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
            if(mesh->HasTextureCoords(i) == true){
                
                //std::cout<<(mesh->HasTextureCoords(0) == true)<<std::endl;
                UVW = mesh->mTextureCoords[0][j];
            }
            else{
                
                UVW = aiVector3D(0,0,0);
            }
            //uvs.push_back(glm::vec2(UVW.x, UVW.y));
            //std::cout<<i<<std::endl;
            if (mesh->HasNormals()){
                n = mesh->mNormals[j];
            }
            else{
                n = aiVector3D(0.0,0.0,0.0);
            }
            
            //normals.push_back(glm::vec3(n.x, n.y, n.z));

            vertices.push_back(Vertex{glm::vec3(pos.x, pos.y, pos.z), glm::vec2(UVW.x, UVW.y), glm::vec3(n.x, n.y, n.z)});
        }

        indices.reserve(3*mesh->mNumFaces);
        for (unsigned int j = 0; j < mesh->mNumFaces; j++)
        {
            indices.push_back(mesh->mFaces[j].mIndices[0]);
            indices.push_back(mesh->mFaces[j].mIndices[1]);
            indices.push_back(mesh->mFaces[j].mIndices[2]);
        }

        meshes.push_back(Mesh(vertices, indices));


    }
    

    return true;
}

bool loadAssImpVert(const char* path, std::vector<unsigned int>& indices, std::vector<Vertex>& vertices)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

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
