#include "ModelLoader.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//Prevzato z learnopengl.com/model-loading

std::vector<Texture> LoadTextures(aiMaterial *mtl, aiTextureType type, std::string typeName, std::string &dir)
{
	std::vector<Texture> textures;
	for (uint32_t i = 0; i < mtl->GetTextureCount(type); ++i)
	{
		aiString path;
		Texture tex;
		int width, height, channels;

		mtl->GetTexture(type, i, &path);
		stbi_set_flip_vertically_on_load(true);
		tex.data = stbi_load((dir + "/" + path.C_Str()).c_str(), &width, &height, &channels, 3);
		if(tex.data == nullptr)
		{
			continue;
		}

		tex.height = height;
		tex.width = width;
		tex.channels = channels;
		tex.type = typeName;
		tex.path = path.C_Str();
		textures.push_back(tex);
	}
	return textures;
}

Mesh* ProcessTreeMesh(const aiScene* scene, aiMesh* mesh, std::string& dir)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	aiMaterial *mtl;
	Material myMaterial;
	std::vector<Texture> textures;

	glm::vec3 tmp;
	for (unsigned i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex v;
		tmp.x = mesh->mVertices[i].x;
		tmp.y = mesh->mVertices[i].y;
		tmp.z = mesh->mVertices[i].z;
		v.position = tmp;

		tmp.x = mesh->mNormals[i].x;
		tmp.y = mesh->mNormals[i].y;
		tmp.z = mesh->mNormals[i].z;
		v.normal = tmp;

		if (mesh->mTextureCoords[0])
		{
			v.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
		{
			v.texCoords = glm::vec2(0.f, 0.f);
		}
		vertices.push_back(v);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		mtl = scene->mMaterials[mesh->mMaterialIndex];
		aiColor4D diffuse;
		aiColor4D ambient;
		aiColor4D specular;
		aiColor4D emissive;
		float shininess;

		aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
		aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient);
		aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular);
		aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emissive);
		aiGetMaterialFloat(mtl, AI_MATKEY_SHININESS, &shininess);

		myMaterial.ambientColor = (glm::f32vec4(ambient.r, ambient.g, ambient.b, ambient.a) == glm::f32vec4(0)) ? glm::f32vec3(AMBIENT_LIGHT) : glm::f32vec4(ambient.r, ambient.g, ambient.b, ambient.a);
		myMaterial.diffuseColor = glm::f32vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
		myMaterial.specluarColor = glm::f32vec4(specular.r, specular.g, specular.b, specular.a);
		myMaterial.emissiveColor = glm::f32vec4(emissive.r, emissive.g, emissive.b, emissive.a);
		myMaterial.shininess = shininess;

		std::vector<Texture> diffMap = LoadTextures(mtl, aiTextureType_DIFFUSE, "texture_diffuse",dir);
		textures.insert(textures.end(), diffMap.begin(), diffMap.end());
		std::vector<Texture> specMap = LoadTextures(mtl, aiTextureType_SPECULAR, "texture_specular",dir);
		textures.insert(textures.end(), specMap.begin(), specMap.end());
	}

	return new Mesh(vertices, indices, false, myMaterial, 0.18f, textures);
}

void ProcessSceneTree(const aiScene* scene, std::vector<Mesh*> &meshes, aiNode* node, std::string& dir)
{
	for (uint32_t i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessTreeMesh(scene, mesh,dir));
	}

	for (uint32_t i = 0; i < node->mNumChildren; ++i)
	{
		ProcessSceneTree(scene, meshes, node->mChildren[i],dir);
	}
}

void LoadScene(std::string& modelPath, std::vector<Mesh*>& meshes)
{
	Assimp::Importer importer;
	const aiScene* scene = nullptr;
	scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_GenNormals);
	if (!scene || !scene->mRootNode || scene->mFlags & (AI_SCENE_FLAGS_INCOMPLETE | AI_SCENE_FLAGS_VALIDATION_WARNING))
	{
		std::cerr << "Model importer failed. flags: " << scene->mFlags << std::endl;
	}
	std::string dir = modelPath.substr(0, modelPath.find_last_of('/'));
	aiNode* node = scene->mRootNode;
	ProcessSceneTree(scene, meshes, node, dir);
}
