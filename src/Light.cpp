#include "Light.h"

Light::Light(float intensity, glm::f32vec3 color)
	: m_Color(color),
	  m_Intensity(intensity)
{}

Light::~Light()
{}

PointLight::PointLight(glm::vec3 pos, float intensity, glm::f32vec3 color)
	: Light(intensity, color),
	  m_Position(pos)
{}

void PointLight::Shine(glm::vec3& lightIntensity, float& lightDistance, glm::vec3& lightDir, glm::vec3& P)
{
	float distanceSquared;
	lightDir = (P - m_Position);
	distanceSquared = Norm(lightDir);
	lightIntensity = m_Color * m_Intensity / glm::vec3((float)(4.f*M_PI*(distanceSquared)));
	lightDistance = sqrt(distanceSquared);
	//vyuziju potreby vzdalenosti a znormalizuju smer svetla
	lightDir[0] /= lightDistance;
	lightDir[1] /= lightDistance;
	lightDir[2] /= lightDistance;
}

DistantLight::DistantLight(glm::vec3 dir, float intensity, glm::f32vec3 color)
	: Light(intensity, color),
	m_Direction(dir)
{}

void DistantLight::Shine(glm::vec3& lightIntensity, float& lightDistance, glm::vec3& lightDir, glm::vec3& P)
{
	lightDir = m_Direction;
	lightIntensity = m_Color * m_Intensity;
	lightDistance = inf;
}
