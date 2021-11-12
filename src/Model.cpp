#include"Model.h"

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

Model::Model() 
{
	
}

Model::Model(const char* file)
{
	// Make a JSON object
    bool res = loadAssImpMeshes(file, meshes);
}

Model::~Model() 
{
    
}

void Model::Draw(Shader& shader, Camera& camera)
{
	// Go over all meshes and draw each one
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Mesh::Draw(shader, camera);
	}
}

void Model::AddMeshTexture(Texture& texture, int position) 
{
    if (position < meshes.size())
    {
        meshes[position].AddTexture(texture);
    }
    
    
}

void Model::AddMesh(Mesh& mesh) 
{
	meshes.push_back(mesh);
}

