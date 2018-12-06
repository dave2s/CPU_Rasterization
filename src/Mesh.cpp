#include "Mesh.h"
#include <algorithm>
//#include "Camera.h"

Mesh::Mesh()
{

}

Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, unsigned int _vertices_len, unsigned int _indices_len, bool _single_sided, Mesh::Material _material, float _albedo, std::vector<Texture> _textures)
{
	vertices = _vertices;
	indices = _indices;
	albedo = _albedo;
	material = _material;
	indices_len = _indices_len;
	single_sided = _single_sided;
	textures = _textures;
}

const glm::uvec2* Mesh::computeTriangleBoundingBox(glm::uvec2(&pixel_aligned_boundary_points)[2],glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2 ) {
	//for x and y component of the vertex - 2D only
	///need to convert to screenspace first
	//glm::uvec2 pixel_aligned_boundary_points[2];
	glm::vec2 boundary_points[2] = { glm::vec2(inf), glm::vec2(-inf) };
	for (unsigned char i = 0; i < 2; ++i) {
		//min
		if (v0[i] < boundary_points[0][i]) {
			boundary_points[0][i] = v0[i];
		}//max
		if (v0[i] > boundary_points[1][i]) {
			boundary_points[1][i] = v0[i];
		}
		//min
		if (v1[i] < boundary_points[0][i]) {
			boundary_points[0][i] = v1[i];
		}//max
		if (v1[i] > boundary_points[1][i]) {
			boundary_points[1][i] = v1[i];
		}
		//min
		if (v2[i] < boundary_points[0][i]) {
			boundary_points[0][i] = v2[i];
		}//max
		if (v2[i] > boundary_points[1][i]) {
			boundary_points[1][i] = v2[i];
		}
	}
	for (unsigned char i = 0; i < 2; ++i) {
		pixel_aligned_boundary_points[i].x = std::max(0,std::min(WIDTH-1,(int)std::floor(boundary_points[i].x)));
		pixel_aligned_boundary_points[i].y = std::max(0, std::min(HEIGHT - 1, (int)std::floor(boundary_points[i].y)));
	}

	return pixel_aligned_boundary_points;
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
return > 0 if c is on the left side of v0-v1 line
return 0 if c lays on v0-v1 line
return <0 if c lays on the right side of v0-v1 line
flip vertices v0 and v1 to invert the return values
*/
float Mesh::edgeFunction(glm::vec3 v0, glm::vec3 v1, glm::vec2 c) {
	return (v1.y - v0.y)*(c.x - v0.x)-(v1.x - v0.x)*(c.y - v0.y);
}

float Mesh::stepEdgeFunction(float prev_edge, float edge_step_x, float edge_step_y) {
	return prev_edge + edge_step_y + edge_step_x;
}

void Mesh::calcFragmentProperties(Vertex &v0, Vertex &v1, Vertex &v2, glm::vec2 &uv, glm::vec3 &N, glm::vec2 &texture_coords) {
#ifndef SMOOTH_SHADING
	glm::f32vec3 N = calcTriangleNormal(v0.position, v1.position, v2.position);
#else
	//Interpolate vertex normals using barycentric coordinates.
	N =glm::normalize( (1-uv.x-uv.y)*v0.normal + uv.x*v1.normal + uv.y*v2.normal );

	texture_coords = ((1 - uv.x - uv.y)*v0.tex_coords + uv.x*v1.tex_coords + uv.y*v2.tex_coords);
#endif
}

bool Mesh::isPixelInTriangle(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2,float &parallelogram_area, glm::vec2 pixel, glm::vec2 &uv, float &z) {
#ifndef SMOOTH_SHADING
	glm::f32vec3 N = calcTriangleUnNormal(v0, v1, v2);
#else
	//For backface culling we use NOT interpolated normal which is same for the whole triangle
	glm::f32vec3 N = calcTriangleUnNormal(v0, v1, v2);
#endif
#ifdef BACKFACE_CULLING
	if (glm::dot(N, (glm::vec3(pixel, -inf) - glm::vec3(0))) < 0) {
		return false;
	}
#endif
	//expecting counter clockwise triangle indices
	///TODO edge overlap - convention is that left or topleft edge is priority...overlap occures when edge function returns 0, so if edgeFun()
	float t, u, v;
	t = edgeFunction(v1, v2, pixel); //rozdil obdelniku, mozno nahlizet jako na kartezsky soucin ve 2d
	u = edgeFunction(v2, v0, pixel); //nebo taky determinant matice kde na prvnim radku je deltax a deltay
	v = edgeFunction(v0, v1, pixel); //pixelu s v0 a na druhem deltax a deltay vrcholu v0 v1.
	
	if (u < 0 || v < 0 || t < 0) return false;
	//if point is laying on the edge, verify that the edge is topleft, otherwise return false -  pixel is NOT overlapping the triangle. This convention prevents edge overlap
	if (t == 0) {
		glm::vec2 edge1 = glm::vec2(v2 - v1);
		if (edge1.y < 0 || (edge1.y == 0 && edge1.x < 0));
		return false;
	}
	if (u == 0) {
		glm::vec2 edge2 = glm::vec2(v0 - v2);
		if (edge2.y < 0 || (edge2.y == 0 && edge2.x < 0));
		return false;
	}
	if (v == 0) {
		glm::vec2 edge0 = glm::vec2(v1 - v0);
		if (edge0.y < 0 || (edge0.y == 0 && edge0.x < 0));
		return false;
	}
	//barycentric coordss
	t /= parallelogram_area;
	uv.x = u/parallelogram_area;
	uv.y = v/parallelogram_area;

	//pixel depth in camera space
	z = 1/(t/v0.z + u/v1.z + v/v2.z);

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
