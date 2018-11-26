#pragma once
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#define AMBIENT_LIGHT 0.075f
class ModelLoader
{
public:
	ModelLoader();

	/*struct Texture {
		unsigned int id;
		std::string path;
		std::string type;
	};*/

	static std::vector<Mesh::Texture> textures;

	static void loadScene(std::string model_path, std::vector<Mesh*>& meshes);
	//void processSceneTree(const aiScene* scene, std::vector<RT_Mesh*> meshes);


	~ModelLoader();
private:

	//const aiScene* scene;
	static void processSceneTree(const aiScene* scene, std::vector<Mesh*>& meshes, aiNode* node);
	static Mesh* processTreeMesh(const aiScene* scene, aiMesh* mesh);
	//	std::vector<RT_Mesh::Texture> loadTextures();

		//std::vector<RT_Mesh::Texture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName);
};




