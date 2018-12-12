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


Camera::Camera(glm::vec3 origin, glm::vec3 front, float _fovy, float _aspect_ratio)
{
	aspect_ratio = _aspect_ratio;
	position = origin;
	view_matrix = glm::lookAt(position, position + front, glm::vec3(0.f, 1.f, 0.f));

	//std::cout << "Aspect ratio:" << std::to_string(TO_RADIANS(fovy)) << std::endl;
	//projection_matrix = glm::perspective(TO_RADIANS(fovy),_aspect_ratio,CAM_NEAR_PLANE, CAM_FAR_PLANE);
	fovy = _fovy;
	scale = glm::tan(TO_RADIANS(fovy*0.5));
	yaw = TO_RADIANS(90.f);
	pitch = TO_RADIANS(0.f);
	Update(front);
}
void Camera::Update(glm::vec3 direction) {
#ifdef DEBUG
	std::cout << glm::to_string(camera_position) << std::endl;
#endif
	glm::vec3 rotation(cos(yaw), sin(pitch), 0);
	view_matrix = glm::lookAt(position, position + direction + rotation, glm::vec3(0.f, 1.f, 0.f));
}

Camera::~Camera()
{
}
