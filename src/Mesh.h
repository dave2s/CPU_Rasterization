#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Camera.h"
#include "Defines.h"

//const float inf = std::numeric_limits<float>::max();
class Mesh
{

public:
	Mesh();

	struct Vertex {
		glm::f32vec3 position;
		glm::f32vec3 normal;
		glm::f32vec2 tex_coords;
	};

	struct Material {
		glm::f32vec3 diffuse_color;
		glm::f32vec3 specluar_color;
		glm::f32vec3 ambient_color;
		float shininess;
	};

	struct Texture {
		unsigned char* data;
		uint32_t id;
		std::string type;
		std::string path;
		int width;
		int height;
		int channels;
		//data?
	};

	float albedo;
	Material material;
	std::vector<Texture> textures;

	//glm::vec2 boundary_points[2] = { glm::vec2(inf), glm::vec2(-inf) };

	bool isSingleSided() { return single_sided; }
	uint32_t getTriangleCount() { return indices_len/3; }

	//Return triangle by index of the triangle
	Vertex* getTriangle(unsigned int idx) {
		Vertex* triangle = new Vertex[3];
		triangle[0] = vertices[indices[0 + 3 * idx]];
		triangle[1] = vertices[indices[1 + 3 * idx]];
		triangle[2] = vertices[indices[2 + 3 * idx]];
		return triangle;
	}

	static glm::f32vec3 calcTriangleUnNormal(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2) {
		return glm::cross(v1-v0, v2-v0);
	}
	static glm::f32vec3 calcTriangleNormal(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2) {
		return glm::normalize(glm::cross(v1 - v0, v2 - v0));
	}

	Mesh(std::vector<Vertex> vertices, std::vector< unsigned int> indices, unsigned int vertices_len, unsigned int indices_len, bool singleSided, Mesh::Material my_material, float albedo, std::vector<Texture> textures);

	static const glm::uvec2* computeTriangleBoundingBox(glm::uvec2(&pixel_aligned_boundary_points)[2],glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2);

	static float edgeFunction(glm::vec3 v0, glm::vec3 v1, glm::vec2 pixel);

	static void calcFragmentProperties(Vertex & v0, Vertex & v1, Vertex & v2, glm::vec3 v0cam, glm::vec3 v1cam, glm::vec3 v2cam, glm::vec2 &uv, float &fragment_depth, int& texture_height, int& texture_width, glm::vec3 &N, glm::vec2 &texture_coords);

	static void calcFragmentProperties(Vertex &v0, Vertex &v1, Vertex &v2, glm::vec3 v0cam, glm::vec3 v1cam, glm::vec3 v2cam, glm::vec2 &uv, float &fragment_depth, glm::vec3 &N);

	static bool isPixelInTriangle(std::vector<float> tuv,glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);

	~Mesh();

private:
	std::vector<unsigned int> indices;
	std::vector<Vertex> vertices;
	uint32_t vertex_count;
	uint32_t indices_len;
	bool single_sided;
};

