#include "Mesh.h"
#include <algorithm>
#include <iostream>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, bool singleSided, Material material, float albedo, std::vector<Texture> textures)
	: m_Albedo(albedo),
	  m_Material(material),
	  m_Textures(textures),
	  m_Indices(indices),
	  m_Vertices(vertices),
	  m_TriangleCount(vertices.size()/3),
	  m_SingleSided(singleSided)
{}

void ComputeTriangleBoundingBox(glm::uvec2(&boundaryAligned)[2], glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2)
{
	glm::vec2 boundary[2] = { glm::vec2(inf), glm::vec2(-inf) };
	for (unsigned char i = 0; i < 2; ++i)
	{
		boundary[0][i] = std::min(boundary[0][i], v0[i]);
		boundary[1][i] = std::max(boundary[1][i], v0[i]);
		boundary[0][i] = std::min(boundary[0][i], v1[i]);
		boundary[1][i] = std::max(boundary[1][i], v1[i]);
		boundary[0][i] = std::min(boundary[0][i], v2[i]);
		boundary[1][i] = std::max(boundary[1][i], v2[i]);
	}
	for (unsigned char i = 0; i < 2; ++i)
	{
		boundaryAligned[i].x = glm::clamp((int)std::floor(boundary[i].x), 0, WIDTH - 1);
		boundaryAligned[i].y = glm::clamp((int)std::floor(boundary[i].y), 0, HEIGHT - 1);
	}
}

void CalcFragmentProperties(Vertex& v0, Vertex& v1, Vertex& v2, glm::vec3 v0cam, glm::vec3 v1cam, glm::vec3 v2cam, glm::vec2& uv, float& fragmentDepth, int& textureHeight, int& textureWidth, glm::vec3& N, glm::vec2 &textureCoords)
{
#ifndef SMOOTH_SHADING
    N = CalcTriangleNormal(v0cam, v1cam, v2cam);
#else
	//Interpolate vertex normals using barycentric coordinates.
	//Perspective correction: divide by respective v.z then multiply by fragment.z
	N.x = (fragmentDepth*((1 - uv.x - uv.y)*v0.normal.x / v0.position.z + uv.x*v1.normal.x / v1.position.z + uv.y*v2.normal.x / v2.position.z));
	N.y = (fragmentDepth*((1 - uv.x - uv.y)*v0.normal.y / v0.position.z + uv.x*v1.normal.y / v1.position.z + uv.y*v2.normal.y / v2.position.z));
	N.z = (fragmentDepth*((1 - uv.x - uv.y)*v0.normal.z / v0.position.z + uv.x*v1.normal.z / v1.position.z + uv.y*v2.normal.z / v2.position.z));
	N = glm::normalize(N);
#endif
	///Textures
	//interpolation with perspective correction (divide by respective vertex depth in camera space, then multiply by fragment depth)
	textureCoords.x = fragmentDepth*((1 - uv.x - uv.y)*v0.tex_coords.x/v0.position.z + uv.x*v1.tex_coords.x/v1.position.z + uv.y*v2.tex_coords.x/v2.position.z);
	textureCoords.y = fragmentDepth*((1 - uv.x - uv.y)*v0.tex_coords.y/v0.position.z + uv.x*v1.tex_coords.y/v1.position.z + uv.y*v2.tex_coords.y/v2.position.z);
#ifdef TEXTURE_REPEAT
	//lets repeat the texture :) /// if 1, rets 0, is ok?
	if (textureCoords.x < 0)
		textureCoords.x = 1 + textureCoords.x - (int)textureCoords.x;
	else
		textureCoords.x = textureCoords.x - (int)textureCoords.x;
	if(textureCoords.y < 0)
		textureCoords.y = 1 + textureCoords.y - (int)textureCoords.y;
	else
		textureCoords.y = textureCoords.y - (int)textureCoords.y;

#else
	textureCoords = glm::clamp(textureCoords,0.f,1.f);
#endif
	///Let's expand the coords from (0,1) to (0,texture_size);
	textureCoords.x = textureCoords.x*textureWidth;
	textureCoords.y = textureCoords.y*textureHeight;
}

void CalcFragmentProperties(Vertex& v0, Vertex& v1, Vertex& v2, glm::vec3 v0cam, glm::vec3 v1cam, glm::vec3 v2cam, glm::vec2& uv, float& fragmentDepth, glm::vec3& N)
{
#ifndef SMOOTH_SHADING
	N = CalcTriangleNormal(v0cam, v1cam, v2cam);
#else
	//Interpolate vertex normals using barycentric coordinates.
	//Perspective correction: divide by respective v.z then multiply by fragment.z
	N.x = (fragmentDepth*((1 - uv.x - uv.y)*v0.normal.x / v0.position.z + uv.x*v1.normal.x / v1.position.z + uv.y*v2.normal.x / v2.position.z));
	N.y = (fragmentDepth*((1 - uv.x - uv.y)*v0.normal.y / v0.position.z + uv.x*v1.normal.y / v1.position.z + uv.y*v2.normal.y / v2.position.z));
	N.z = (fragmentDepth*((1 - uv.x - uv.y)*v0.normal.z / v0.position.z + uv.x*v1.normal.z / v1.position.z + uv.y*v2.normal.z / v2.position.z));
#endif
}
