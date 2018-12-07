#pragma once
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"

class ModelLoader
{
public:
	ModelLoader();

	static void loadScene(std::string& model_path, std::vector<Mesh*>& meshes/*, std::vector<Mesh::Texture>& loaded_textures*/);

	~ModelLoader();
private:

	static void processSceneTree(const aiScene* scene, std::vector<Mesh*>& meshes, aiNode* node, std::string& dir/*, std::vector<Mesh::Texture>& loaded_textures*/);
	static Mesh* processTreeMesh(const aiScene* scene, aiMesh* mesh, std::string& dir/*, std::vector<Mesh::Texture>& loaded_textures*/);
	static std::vector<Mesh::Texture> loadTextures(aiMaterial *mtl, aiTextureType type, std::string typeName, std::string &dir/*, std::vector<Mesh::Texture>& loaded_textures*/);
	static uint32_t loadTextureFile(unsigned char** img_data,const char* path, std::string &dir, int& width, int& height, int& channels);
};