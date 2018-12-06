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
		//data?
	};

	float albedo;
	Material material;
	std::vector<Texture> textures;

	//glm::vec2 boundary_points[2] = { glm::vec2(inf), glm::vec2(-inf) };

	bool isSingleSided() { return single_sided; };
	int getTriangleCount() { return indices_len / 3; }

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

	static float stepEdgeFunction(float prev_edge, float edge_step_x, float edge_step_y);

	static void calcFragmentProperties(Vertex & v0, Vertex & v1, Vertex & v2, glm::vec2 &uv, glm::vec3 &N, glm::vec2 &texture_coords);

	static bool isPixelInTriangle(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2,float &parallelogram_area, glm::vec2 pixel, glm::vec2 &uv ,float &z);

	~Mesh();

private:
	std::vector<unsigned int> indices;
	std::vector<Vertex> vertices;
	uint16_t vertex_count;
	uint16_t indices_len;
	bool single_sided;
};

