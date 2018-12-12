#pragma once
#define GLM_LEFT_HANDED
#include "glm/glm.hpp"

#define CAM_NEAR_PLANE 0.5f
#define CAM_FAR_PLANE 500.0f
#define COORDS_FLOAT_WIDTH 2.0f
#define COORDS_FLOAT_HEIGHT 2.0f

#ifndef M_PI
#define M_PI 3.14159265358979323846264f
#endif

#define TO_RADIANS(x)  x*M_PI/180.f
const float DEG90 = TO_RADIANS(90);
const float ROTSTEP = 15;
const float MOVSTEP = 0.2f;

const float inf = std::numeric_limits<float>::max();

class Camera
{
public:
	Camera(glm::vec3 origin, float pitch, float yaw, float fovy, float aspect_ratio);

	void ChangePosition(glm::vec3 difference);
	void ChangeRotation(float pitch, float yaw);
	void Reset();

	float& GetAspectRatio();
	float& GetScale();
	glm::mat4& GetViewMatrix();
	glm::vec3& GetPosition();

private:
	void Update();

	glm::mat4 m_ViewMatrix;
	glm::vec3 m_Position;
	glm::vec3 m_Origin;
	float m_PitchOrigin;
	float m_YawOrigin;
	float m_AspectRatio;
	float m_Scale;
	float m_Pitch;
	float m_Yaw;
};
