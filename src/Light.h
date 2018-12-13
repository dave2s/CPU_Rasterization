#pragma once
#include <glm/glm.hpp>
#include "Defines.h"

enum LIGHT_TYPE
{
	LIGHT_NONE = 0,
	LIGHT_POINT = 1,
	LIGHT_DISTANT = 2
};

class Light
{
public:
	Light(float intensity, glm::f32vec3 color);
	virtual ~Light();

	virtual void Shine(glm::vec3& lightIntensity, float& lightDistance, glm::vec3& lightDir, glm::vec3& P) = 0;
	inline virtual LIGHT_TYPE GetType()
	{
		return LIGHT_NONE;
	}
	inline float& GetIntensity()
	{
		return m_Intensity;
	}
	inline glm::vec3& GetColor()
	{
		return m_Color;
	}
protected:
	inline static float Norm(glm::vec3& vec)
	{
		return (vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	}

	glm::vec3 m_Color;
	float m_Intensity;
};

class PointLight : public Light
{
public:
	PointLight(glm::vec3 pos, float intensity, glm::f32vec3 color);

	void Shine(glm::vec3& lightIntensity, float& lightDistance, glm::vec3& lightDir, glm::vec3& P) override;
	inline LIGHT_TYPE GetType()
	{
		return LIGHT_POINT;
	}
	inline glm::vec3& GetPosition()
	{
		return m_Position;
	}
private:
	glm::vec3 m_Position;
};

class DistantLight : public Light
{
public:
	DistantLight(glm::vec3 dir, float intensity, glm::f32vec3 color);

	void Shine(glm::vec3& lightIntensity, float& lightDistance, glm::vec3& lightDir, glm::vec3& P) override;
	inline LIGHT_TYPE GetType()
	{
		return LIGHT_DISTANT;
	}
	inline glm::vec3& GetDirection()
	{
		return m_Direction;
	}
private:
	glm::vec3 m_Direction;
};
