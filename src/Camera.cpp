#include "Camera.h"
#include <string>
#include <iostream>
#include "glm/gtc/matrix_transform.hpp"
//#define DEBUG
#ifdef DEBUG
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#endif

#include <cmath>

Camera::Camera(glm::vec3 origin, float pitch, float yaw, float fovy, float aspectRatio)
	: m_Position(origin),
	  m_Origin(origin),
	  m_PitchOrigin(TO_RADIANS(pitch)),
	  m_YawOrigin(TO_RADIANS(yaw)),
	  m_AspectRatio(aspectRatio),
	  m_Scale(glm::tan(TO_RADIANS(fovy*0.5))),
	  m_Pitch(TO_RADIANS(pitch)),
	  m_Yaw(TO_RADIANS(yaw))
{
	Update();
}

void Camera::ChangePosition(glm::vec3 difference)
{
	m_Position += difference;

	Update();
}

void Camera::ChangeRotation(float pitch, float yaw)
{
	m_Pitch += TO_RADIANS(pitch);
	m_Yaw += TO_RADIANS(yaw);


	m_Pitch = glm::clamp(m_Pitch, m_PitchOrigin-DEG90, m_PitchOrigin+DEG90);
	m_Yaw = glm::clamp(m_Yaw, m_YawOrigin-DEG90, m_YawOrigin+DEG90);

	Update();
}

void Camera::Reset()
{
	m_Position = m_Origin;
	m_Yaw = m_YawOrigin;
	m_Pitch = m_PitchOrigin;

	Update();
}

float& Camera::GetAspectRatio()
{
	return m_AspectRatio;
}

float& Camera::GetScale()
{
	return m_Scale;
}

glm::mat4& Camera::GetViewMatrix()
{
	return m_ViewMatrix;
}

glm::vec3& Camera::GetPosition()
{
	return m_Position;
}

void Camera::Update() {
#ifdef DEBUG
	std::cout << glm::to_string(m_Position) << std::endl;
#endif
	m_ViewMatrix = glm::lookAt(m_Position, m_Position + glm::vec3(cos(m_Yaw), sin(m_Pitch), -1.f), glm::vec3(0.f, 1.f, 0.f));
}
