#include "Mesh.h"

Mesh::Mesh()
{

}

Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, unsigned int _vertices_len, unsigned int _indices_len, bool _singleSided, Mesh::Material my_material, float _albedo)
{
	vertices = _vertices;
	indices = _indices;
	for (unsigned int i = 0; i < _vertices_len; ++i) {
		updateBoundaries(vertices[i]);
	}
	//memcpy(indices, &_indices[0], sizeof(unsigned int)*_indices_len);
	//indices = &_indices[0];
	albedo = _albedo;
	material = my_material;
	//color = material.diffuse_color;
	indices_len = _indices_len;
	singleSided = _singleSided;
	//albedo = glm::f32vec3(_albedo);
	//material_type = _material;
}

void Mesh::updateBoundaries(Vertex &vertex) {
	//for x and y component of the vertex - 2D only
	///need to convert to screenspace first
	/*for (unsigned char i = 0; i < 2; ++i) {
		if (vertex.position[i] < boundary_points[0][i]) {
			boundary_points[0][i] = vertex.position[i];
		}
		if (vertex.position[i] > boundary_points[1][i]) {
			boundary_points[1][i] = vertex.position[i];
		}
	}*/
}
/*
void Mesh::ClearMesh()
{
	//delete(indices);
	//delete(vertices);
	//delete(this);
}*/
/*
bool RT_Mesh::shadowRayHitTriangle(std::vector<glm::vec3> _triangle, Ray *ray, bool _singleSided, float& distance, glm::vec3 & PHit, float min_dist)
{//TODO edit to use references or pointers?
	//std::cout << "kreslim vertexy: v0: " << glm::to_string(triangle[0]) << " a v1: " << glm::to_string(triangle[1]) << " a v2: " << glm::to_string(triangle[2]) << " \n";
	glm::vec3 normal = RT_Mesh::getTriangleNormal(_triangle);//is normalized
	//std::cout <<"normala: "<< glm::to_string(normal) << " vertex 0 = " << glm::to_string(_triangle[0])<<"\n";
	float d = getDistanceFromOrigin(normal, _triangle[0]);
	int e = 0;
	 d = getPlaneIntersectionDistance(d, normal, ray->origin, ray->direction, _singleSided,e);
	///Distance > min_dist <-this triangle is further than previously hit
	if (d <= 0.001f ||d > min_dist||e==1) {
		return false; // Triangle is behind the camera OR it's parallel with the ray OR it's faced the other way and is single sided - both invisible ///TODO too far - fake dust in air
	}
	glm::vec3 _PHit = getPlaneIntersection(ray->origin, d, ray->direction);

	///is Phit inside the triangle? test against each edge
	if (glm::dot(normal, glm::cross(_triangle[1] - _triangle[0], _PHit - _triangle[0])) < 0) { return false; }
	if (glm::dot(normal, glm::cross(_triangle[2] - _triangle[1], _PHit - _triangle[1])) < 0) { return false; }
	if (glm::dot(normal, glm::cross(_triangle[0] - _triangle[2], _PHit - _triangle[2])) < 0) { return false; }
	PHit = _PHit;
	distance = d;
	return true; //successful hit
}*/

/*
return > 0 if c is on the right side of v0-v1 line
return 0 if c lays on v0-v1 line
return <0 if c lays on the left side of v0-v1 line
*/
float Mesh::edgeFunction(glm::vec3 v0, glm::vec3 v1, glm::vec2 c) {
	return (v1.x - v0.x)*(c.y - v0.y) - (v1.y - v0.y)*(c.x - v0.x);
}

float Mesh::stepEdgeFunction(float prev_edge, float edge_step_x, float edge_step_y) {
	return prev_edge + edge_step_y + edge_step_x;
}

bool Mesh::isPixelInTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 pixel, float &u, float &v,float &z) {
#ifndef SMOOTH_SHADING
	//glm::f32vec3 N = calcTriangleUnNormal(v0, v1, v2);
#else
	glm::fvec3 N = calcTriangleNormalSmooth(_triangle);
#endif
	//expecting counter clockwise triangle indices
	if (edgeFunction(v0, v1, pixel) < 0) return false;
	if (edgeFunction(v1, v2, pixel) < 0) return false;
	if (edgeFunction(v2, v0, pixel) < 0) return false;
	return true;
}
/*
bool Mesh::pixelInTriangle(glm::vec3* _triangle,bool isPrimary, bool _singleSided,float& distance, glm::vec2 &pixel,float min_dist)
{//TODO edit to use references or pointers?

	glm::vec3 normal = Mesh::calcTriangleNormal(_triangle);//is normalized
	//float d = getDistanceFromOrigin(normal, _triangle[0]);
	//int e = 0;
	//parametr t z P=O+tR
	//d = (isPrimary ? 1.f : -1.f)* getPlaneIntersectionDistance(isPrimary,d, normal, ray->origin, ray->direction, _singleSided,e);
	///Distance > min_dist <-this triangle is further than previously hit
	//d *= (isPrimary? 1.f : -1.f);//camera has negative z, ray doesnt...
	//std::cout << "vzdalenost: " << d << std::endl;
	/*if ((d <= (isPrimary? CAM_NEAR_PLANE: 0.f) +0.00f) || (d > min_dist)||(e==1)) {//e==1 odvraceny trojuhelnik,
		return false; // Triangle is behind the camera OR it's parallel with the ray OR it's faced the other way and is single sided - both invisible
	}
	//glm::vec3 _PHit = getPlaneIntersection(ray->origin, d, ray->direction);
	//_PHit = getPlaneIntersection(ray->origin, d, ray->direction);

	///is Phit inside the triangle? test against each edge
	if (glm::dot(normal, glm::cross(glm::vec2(_triangle[1]) - glm::vec2(_triangle[0]), pixel - glm::vec2(_triangle[0]))) < 0) { return false; }
	if (glm::dot(normal, glm::cross(_triangle[2] - _triangle[1], pixel - _triangle[1])) < 0) { return false; }
	if (glm::dot(normal, glm::cross(_triangle[0] - _triangle[2], pixel - _triangle[2])) < 0) { return false; }
	//PHit = _PHit;
	//distance = d;
	return true; //successful hit
}*/

Mesh::~Mesh()
{
	
}
