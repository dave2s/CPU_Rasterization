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
		unsigned int id;
		std::string type;
		std::string path;
	};
	float albedo;
	Material material;

	//glm::vec2 boundary_points[2] = { glm::vec2(inf), glm::vec2(-inf) };

	bool isSingleSided() { return singleSided; };
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
	static glm::f32vec3 calcTriangleNormal(glm::vec3 vertices[3]) {
		return glm::normalize(glm::cross(vertices[1] - vertices[0], vertices[2] - vertices[0]));
	}

	Mesh(std::vector<Vertex> vertices, std::vector< unsigned int> indices, unsigned int vertices_len, unsigned int indices_len, bool singleSided, Mesh::Material my_material, float albedo);

	static const glm::uvec2* computeTriangleBoundingBox(glm::uvec2(&pixel_aligned_boundary_points)[2],glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2);

	static float edgeFunction(glm::vec3 v0, glm::vec3 v1, glm::vec2 pixel);

	static float stepEdgeFunction(float prev_edge, float edge_step_x, float edge_step_y);

	static bool isPixelInTriangle(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2,float &parallelogram_area, glm::vec2 pixel, float &t, float &u, float &v ,float &z);

	~Mesh();

private:
	std::vector<unsigned int> indices;
	std::vector<Vertex> vertices;
	uint16_t vertex_count;
	uint16_t indices_len;
	bool singleSided;
};

