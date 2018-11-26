#pragma once
#include <glm/glm.hpp>
#include <vector>

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

	Material material;

	bool isSingleSided() { return singleSided; };
	int getTriangleCount() { return indices_len / 3; }

	//Return triangle by index of the triangle
	Vertex* getTriangle(unsigned int idx) {
		Vertex triangle[3] = { vertices[indices[0 + 3 * idx]],vertices[indices[1 + 3 * idx]], vertices[indices[2 + 3 * idx]] };
		return triangle;
	}

	static glm::f32vec3 calcTriangleUnNormal(glm::vec3 vertices[3]) {
		return glm::cross(vertices[1] - vertices[0], vertices[2] - vertices[0]);
	}
	static glm::f32vec3 calcTriangleNormal(glm::vec3 vertices[3]) {
		return glm::normalize(glm::cross(vertices[1] - vertices[0], vertices[2] - vertices[0]));
	}

	Mesh(std::vector<Vertex> vertices, std::vector< unsigned int> indices, unsigned int vertices_len, unsigned int indices_len, bool singleSided, Mesh::Material my_material);

	bool pixelInTriangle(glm::vec3* _triangle, bool _singleSided, glm::vec2 pixel, float &u, float &v, float &min_dist);

	~Mesh();

private:
	std::vector<unsigned int> indices;
	std::vector<Vertex> vertices;
	uint16_t vertex_count;
	uint16_t indices_len;
	bool singleSided;
};

