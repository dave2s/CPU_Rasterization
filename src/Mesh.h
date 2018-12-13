#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Camera.h"
#include "Defines.h"

struct Vertex
{
	glm::f32vec3 position;
	glm::f32vec3 normal;
	glm::f32vec2 tex_coords;
};

struct Material
{
	glm::f32vec3 diffuse_color;
	glm::f32vec3 specluar_color;
	glm::f32vec3 ambient_color;
	glm::f32vec3 emissive_color;
	float shininess;
};

struct Texture
{
	unsigned char* data;
	uint32_t id;
	std::string type;
	std::string path;
	int width;
	int height;
	int channels;
};

inline glm::f32vec3 CalcTriangleUnNormal(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2)
{
	return glm::cross(v1-v0, v2-v0);
}

inline glm::f32vec3 CalcTriangleNormal(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2)
{
	return glm::normalize(glm::cross(v1 - v0, v2 - v0));
}

/*
return > 0 if c is on the left side of v0-v1 line
return 0 if c lays on v0-v1 line
return <0 if c lays on the right side of v0-v1 line
flip vertices v0 and v1 to invert the return values
*/
inline float EdgeFunction(glm::vec3 v0, glm::vec3 v1, glm::vec2 c)
{
	return (v1.y - v0.y)*(c.x - v0.x)-(v1.x - v0.x)*(c.y - v0.y);
}

inline void CalcReflectedDirection(glm::vec3& Nhit, glm::vec3& incidentDir)
{
	incidentDir -= 2.f * Nhit*(glm::dot(Nhit, incidentDir));
}

void ComputeTriangleBoundingBox(glm::uvec2(&boundaryAlignet)[2], glm::vec3& v0, glm::vec3& v1, glm::vec3& v2);

void CalcFragmentProperties(Vertex& v0, Vertex& v1, Vertex& v2, glm::vec3 v0cam, glm::vec3 v1cam, glm::vec3 v2cam, glm::vec2& uv, float& fragmentDepth, int& textureHeight, int& textureWidth, glm::vec3& N, glm::vec2& textureCoords);

void CalcFragmentProperties(Vertex &v0, Vertex &v1, Vertex &v2, glm::vec3 v0cam, glm::vec3 v1cam, glm::vec3 v2cam, glm::vec2 &uv, float &fragmentDepth, glm::vec3& N);

inline bool CheckIncorrectEdge(glm::vec2 edge)
{
	return (edge.y > 0 && (edge.y != 0 || edge.x > 0));
}

inline bool IsPixelInTriangle(std::vector<float> tuv, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
	//if point is laying on the edge, verify that the edge is topleft, otherwise return false -  pixel is NOT overlapping the triangle. This convention prevents edge overlap
	return (!((tuv[0] < 0 || tuv[1] < 0 || tuv[2] < 0)
	   || ((tuv[0] == 0) && (CheckIncorrectEdge(glm::vec2(v2-v1))))
	   || ((tuv[1] == 0) && (CheckIncorrectEdge(glm::vec2(v0-v2))))
	   || ((tuv[2] == 0) && (CheckIncorrectEdge(glm::vec2(v1-v0))))));
}

class Mesh
{

public:
	inline bool IsSingleSided()
	{
		return m_SingleSided;
	}

	inline uint32_t& GetTriangleCount()
	{
		return m_TriangleCount;
	}

	inline Vertex GetTrianglePoint(unsigned int idx, unsigned int idy)
	{
		return m_Vertices[m_Indices[3*idx+idy]];
	}

	inline Material& GetMaterial()
	{
		return m_Material;
	}

	inline float& GetAlbedo()
	{
		return m_Albedo;
	}

	inline std::vector<Texture>& GetTextures()
	{
		return m_Textures;
	}

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, bool singleSided, Material myMaterial, float albedo, std::vector<Texture> textures);

private:
	float m_Albedo;
	Material m_Material;
	std::vector<Texture> m_Textures;
	std::vector<unsigned int> m_Indices;
	std::vector<Vertex> m_Vertices;
	uint32_t m_TriangleCount;
	bool m_SingleSided;
};
