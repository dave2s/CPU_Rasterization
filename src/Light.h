#pragma once
#include <glm/glm.hpp>
#include "Camera.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264f
#endif

class Light
{
public:

	enum LIGHT_TYPE { none = 0, point = 1, distant = 2 };

	Light(float intensity, glm::f32vec3 color);
	virtual ~Light();

	glm::vec3 color;
	float intensity;

	virtual void shine(glm::vec3& light_intensity, float& light_distance, glm::vec3& light_dir, glm::vec3& P);
	virtual LIGHT_TYPE getType() { return none; }
protected:
	//const float inf = std::numeric_limits<float>::max();
	static float norm(glm::vec3 vec) { return (vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]); }
};

class PointLight : public Light
{

public:
	glm::vec3 position;
	PointLight(glm::vec3 pos, float intensity, glm::f32vec3 color);

	void shine(glm::vec3& light_intensity, float& light_distance, glm::vec3& light_dir, glm::vec3& P);
	LIGHT_TYPE getType() { return point; }
};

class DistantLight : public Light
{

public:
	glm::vec3 direction;
	DistantLight(glm::vec3 dir, float intensity, glm::f32vec3 color);

	void shine(glm::vec3& light_intensity, float& light_distance, glm::vec3& light_dir, glm::vec3& P);
	LIGHT_TYPE getType() { return distant; }
};
