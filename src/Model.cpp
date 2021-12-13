#include"Model.h"
#include <iostream>
#include <filesystem>
#include <sys/stat.h>

inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from)
{
    glm::mat4 to;


    to[0][0] = (GLfloat)from->a1; to[0][1] = (GLfloat)from->b1;  to[0][2] = (GLfloat)from->c1; to[0][3] = (GLfloat)from->d1;
    to[1][0] = (GLfloat)from->a2; to[1][1] = (GLfloat)from->b2;  to[1][2] = (GLfloat)from->c2; to[1][3] = (GLfloat)from->d2;
    to[2][0] = (GLfloat)from->a3; to[2][1] = (GLfloat)from->b3;  to[2][2] = (GLfloat)from->c3; to[2][3] = (GLfloat)from->d3;
    to[3][0] = (GLfloat)from->a4; to[3][1] = (GLfloat)from->b4;  to[3][2] = (GLfloat)from->c4; to[3][3] = (GLfloat)from->d4;

    return to;
}

// https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-14-17-c
inline bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}



Model::Model() 
{
	
}

Model::Model(std::string model, std::string resources)
    : model_path(model), resources_path(resources)
{
	// Make a JSON object
    bool res = loadModel();
}

Model::~Model() 
{
    
}

void Model::Draw(Shader& shader, Camera& camera, glm::vec3 scale, glm::vec3 translate)
{
	// Go over all meshes and draw each one
	for (unsigned int i = 0; i < meshes.size(); i++)
	{        
        // POSSIBLE SPEED DECREASE MAYBE REDO
        if(meshes[i].GetOpacity() == 1.0){
            //std::cout<<i<<std::endl;
            meshes[i].Mesh::Draw(shader, camera, scale, translate);
        }
	}
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    for (unsigned int i = 0; i < meshes.size(); i++)
	{
        // POSSIBLE SPEED DECREASE MAYBE REDO
        if(meshes[i].GetOpacity() != 1.0){
            meshes[i].Mesh::Draw(shader, camera, scale, translate);
        }
	}

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    for (unsigned int i = 0; i < meshes.size(); i++)
	{
        // POSSIBLE SPEED DECREASE MAYBE REDO
        if(meshes[i].GetOpacity() != 1.0){
            meshes[i].Mesh::Draw(shader, camera, scale, translate);
        }
	}

    glDisable(GL_CULL_FACE);
}

void Model::AddMeshTexture(Texture& texture, int position) 
{
    if (position < meshes.size())
    {
        meshes[position].AddTexture(texture);
    }
    else
    {
        std::cout<<"Couldn't add Texture." <<std::endl;
    }
    
    
}

void Model::AddMesh(Mesh& mesh) 
{
	meshes.push_back(mesh);
}

void Model::processMesh(aiMesh *mesh, const aiScene *scene, aiMatrix4x4 accTransform)
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

    meshes.push_back(Mesh(vertices, indices));

    // Set the node transformation matrix
    meshes[meshes.size() - 1].SetModelMatrix(aiMatrix4x4ToGlm(&accTransform));

    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        aiString texture_file;
        // TRY TEXTURES
        if (material->GetTextureCount(aiTextureType_AMBIENT))
        {
            material->GetTexture(aiTextureType_AMBIENT, 0, &texture_file);
            std::cout<<"Ambient texture"<<std::endl;
            std::cout<<"Mtl name: " << material->GetName().C_Str() << std::endl;
            std::cout<<"Texture file: " << texture_file.C_Str() << std::endl;
        }
        else if (material->GetTextureCount(aiTextureType_SPECULAR))
        {
            material->GetTexture(aiTextureType_SPECULAR, 0, &texture_file);
            std::cout<<"Specular texture"<<std::endl;
            std::cout<<"Mtl name: " << material->GetName().C_Str() << std::endl;
            std::cout<<"Texture file: " << texture_file.C_Str() << std::endl;
        }
        else if (material->GetTextureCount(aiTextureType_DIFFUSE))
        {
            material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_file);
            std::cout<<"Diffuse texture"<<std::endl;
            std::cout<<"Mtl name: " << material->GetName().C_Str() << std::endl;
            std::cout<<"Texture file: " << texture_file.C_Str() << std::endl;
        }
        else if (material->GetTextureCount(aiTextureType_UNKNOWN))
        {
            material->GetTexture(aiTextureType_UNKNOWN, 0, &texture_file);
            std::cout<<"Unknown texture"<<std::endl;
            std::cout<<"Mtl name: " << material->GetName().C_Str() << std::endl;
            std::cout<<"Texture file: " << texture_file.C_Str() << std::endl;
        }
        // TRY COLORS
        else{
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            aiColor4D ambient (0.0f, 0.0f, 0.0f, 0.0f);
            aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient);
            aiColor4D diffuse (0.0f, 0.0f, 0.0f, 0.0f);
            aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
            aiColor4D specular (0.0f, 0.0f, 0.0f, 0.0f);
            aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular);

            float opacity = 0.0;
            material->Get(AI_MATKEY_OPACITY, opacity);

            glm::vec3 ambientV = {ambient.r, ambient.g, ambient.b};
            glm::vec3 diffuseV = {diffuse.r, diffuse.g, diffuse.b};
            glm::vec3 specularV = {specular.r, specular.g, specular.b};

            meshes[meshes.size() - 1].SetColors(ambientV, diffuseV, specularV);
            meshes[meshes.size() - 1].SetOpacity(opacity);

            return;
        }

        // Find the texture and create it
        if(auto texture = scene->GetEmbeddedTexture(texture_file.C_Str())) {
            //returned pointer is not null, read texture from memory
            std::cout<<"Got a embedded texture!"<<std::endl;
        } 
        else {
            std::string fileStr(texture_file.C_Str());
            std::cout<<"Texture should be in a file, go check it!"<<std::endl;
            
            std::string tex_path = resources_path + fileStr;
            std::cout<<tex_path<<std::endl;

            if (!file_exists(tex_path))
            {
                std::cout<<"ERROR: Impossible to find the texture: "<<tex_path<<std::endl;
                return;
            }
            

            // Watch out for more textures? TODO
            Texture modelTexture(tex_path, 1);

            std::cout<<meshes.size() - 1<<std::endl;

            meshes[meshes.size() - 1].AddTexture(modelTexture);

        }
    }
}

void Model::processNode(aiNode *node, const aiScene *scene, aiMatrix4x4 accTransform)
{
    // process all the node's meshes (if any)

    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        processMesh(mesh, scene, accTransform);	
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, accTransform * node->mChildren[i]->mTransformation);
    }
}


bool Model::loadModel()
{
    Assimp::Importer import;
    std::cout<<"before scene loaded"<<std::endl;
    const aiScene *scene = import.ReadFile(model_path.c_str(), aiProcess_Triangulate);
    std::cout<<"after scene loaded"<<std::endl;

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return false;
    }

    processNode(scene->mRootNode, scene, scene->mRootNode->mTransformation);

    return true;

}


