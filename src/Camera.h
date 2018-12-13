#pragma once
#define GLM_LEFT_HANDED
#include "glm/glm.hpp"
#include "Defines.h"

#define CAM_NEAR_PLANE 0.5f
#define CAM_FAR_PLANE 500.0f
#define COORDS_FLOAT_WIDTH 2.0f
#define COORDS_FLOAT_HEIGHT 2.0f

const float DEG90 = TO_RADIANS(90);
const float ROTSTEP = 15;
const float MOVSTEP = 0.2f;

class Camera
{
public:
	Camera(glm::vec3 origin, float pitch, float yaw, float fovy, float aspect_ratio);

	void ChangePosition(glm::vec3 difference);
	void ChangeRotation(float pitch, float yaw);
	void Reset();

	inline float& GetAspectRatio()
	{
		return m_AspectRatio;
	}

	inline float& GetScale()
	{
		return m_Scale;
	}

	inline glm::mat4& GetViewMatrix()
	{
		return m_ViewMatrix;
	}

	inline glm::vec3& GetPosition()
	{
		return m_Position;
	}

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
