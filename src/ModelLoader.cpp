#include <iostream>
#include "ModelLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//Prevzato z learnopengl.com/model-loading
//

ModelLoader::ModelLoader()
{
}

void ModelLoader::loadScene(std::string& model_path, std::vector<Mesh*>& meshes/*, std::vector<Mesh::Texture>& loaded_textures*/) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(model_path, aiProcess_Triangulate | aiProcess_GenNormals /*| aiProcess_FlipUVs*/);
	if (!scene || !scene->mRootNode || scene->mFlags & (AI_SCENE_FLAGS_INCOMPLETE | AI_SCENE_FLAGS_VALIDATION_WARNING)) {
		std::cout << "Model importer failed. flags: " << scene->mFlags << std::endl;
	}
	std::string dir = model_path.substr(0, model_path.find_last_of('/'));
	aiNode* node = scene->mRootNode;
	processSceneTree(scene, meshes, node, dir/*,loaded_textures*/);
}

void ModelLoader::processSceneTree(const aiScene* scene, std::vector<Mesh*> &meshes, aiNode* node, std::string& dir/*, std::vector<Mesh::Texture>& loaded_textures*/) {
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processTreeMesh(scene, mesh,dir/*,loaded_textures*/));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processSceneTree(scene, meshes, node->mChildren[i],dir/*,loaded_textures*/);
	}
}

Mesh* ModelLoader::processTreeMesh(const aiScene* scene, aiMesh* mesh, std::string& dir/*, std::vector<Mesh::Texture>& loaded_textures*/) {
	std::vector< Mesh::Vertex >vertices;
	std::vector<unsigned int> indices;
	aiMaterial *mtl;
	Mesh::Material my_material;
	std::vector<Mesh::Texture> textures;

	//RT_Mesh::MATERIAL_TYPE type = RT_Mesh::DIFFUSE;
	//Mesh::MATERIAL_TYPE type = Mesh::PHONG;


	glm::vec3 tmp;
	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		Mesh::Vertex v;
		tmp.x = mesh->mVertices[i].x;
		tmp.y = mesh->mVertices[i].y;
		tmp.z = mesh->mVertices[i].z;
		v.position = tmp;

		tmp.x = mesh->mNormals[i].x;
		tmp.y = mesh->mNormals[i].y;
		tmp.z = mesh->mNormals[i].z;
		v.normal = tmp;

		if (mesh->mTextureCoords[0]) {
			v.tex_coords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else {
			v.tex_coords = glm::vec2(0.f, 0.f);
		}
		vertices.push_back(v);
	}
	//indices = new unsigned int[mesh->mNumFaces];
	//indices = new unsigned int[mesh->mNumFaces]();
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
		float shininess;
		aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
		aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient);
		aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular);
		aiGetMaterialFloat(mtl, AI_MATKEY_SHININESS, &shininess);

		///TODO alpha
		my_material.ambient_color = (glm::f32vec4(ambient.r, ambient.g, ambient.b, ambient.a) == glm::f32vec4(0)) ? glm::f32vec3(AMBIENT_LIGHT) : glm::f32vec4(ambient.r, ambient.g, ambient.b, ambient.a);
		my_material.diffuse_color = glm::f32vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
		my_material.specluar_color = glm::f32vec4(specular.r, specular.g, specular.b, specular.a);
		my_material.shininess = shininess;


		std::vector<Mesh::Texture> diff_Map = loadTextures(mtl, aiTextureType_DIFFUSE, "texture_diffuse",dir/*,loaded_textures*/);
		textures.insert(textures.end(), diff_Map.begin(), diff_Map.end());
		std::vector<Mesh::Texture> spec_map = loadTextures(mtl, aiTextureType_SPECULAR, "texture_specular",dir/*,loaded_textures*/);
		textures.insert(textures.end(), spec_map.begin(), spec_map.end());
	}

	Mesh* my_mesh = new Mesh(vertices, indices, vertices.size(), indices.size(), false, my_material, 0.18f, textures);
	return my_mesh;
}

std::vector<Mesh::Texture> ModelLoader::loadTextures(aiMaterial *mtl, aiTextureType type, std::string type_name, std::string &dir/*, *std::vector<Mesh::Texture>& loaded_textures*/) {
	std::vector<Mesh::Texture> textures;
	for (uint32_t i = 0; i < mtl->GetTextureCount(type); ++i) {
		aiString path;
		mtl->GetTexture(type, i, &path);
		bool tex_loaded;
		/*for (uint32_t j = 0; j < loaded_textures.size; ++j) {
			if (std::strcmp(loaded_textures[j].path.data(), path.C_Str()) == 0) {
				textures.push_back(loaded_textures[j]);
				tex_loaded = true;
				break;
			}
		}*/
		//if (!tex_loaded) {
			Mesh::Texture tex;
			//unsigned char* img_data = nullptr;
			int width; int height; int channels;
			tex.id = loadTextureFile(&tex.data, path.C_Str(), dir, width, height, channels);
			if( tex.id==0)
				continue;
			//memcpy(tex.data, img_data, size);
			//stbi_image_free(img_data);
			tex.height = height;
			tex.width = width;
			tex.channels = channels;
			tex.type = type_name;
			tex.path = path.C_Str();
			textures.push_back(tex);
			//loaded_textures.push_back(tex.path);
		//}
	}
	return textures;
}

uint32_t ModelLoader::loadTextureFile(unsigned char** img_data,const char *path, std::string &dir, int& width, int& height, int& channels) {
	//img_data = ???;
	
	*img_data = stbi_load((dir+'/'+std::string(path)).c_str(),&width,&height,&channels,3);
	if (img_data) {
		if (channels != 3){
			stbi_image_free(img_data);
			return 0;
		}
	}
	uint32_t id = 1;
	return id;
}


ModelLoader::~ModelLoader()
{
}
