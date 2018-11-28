#pragma once
#include <glm/glm.hpp>
#include <vector>

class Mesh
{
	const float inf = std::numeric_limits<float>::max();
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

	glm::vec2 boundary_points[2] = { glm::vec2(inf), glm::vec2(-inf) };

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

	Mesh(std::vector<Vertex> vertices, std::vector< unsigned int> indices, unsigned int vertices_len, unsigned int indices_len, bool singleSided, Mesh::Material my_material, float albedo);

	void updateBoundaries(Vertex & vertex);

	static float edgeFunction(glm::vec3 v0, glm::vec3 v1, glm::vec2 pixel);

	static float stepEdgeFunction(float prev_edge, float edge_step_x, float edge_step_y);

	static bool isPixelInTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 pixel, float &u, float &v, float &z);

	~Mesh();

private:
	std::vector<unsigned int> indices;
	std::vector<Vertex> vertices;
	uint16_t vertex_count;
	uint16_t indices_len;
	bool singleSided;
};

