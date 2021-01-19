#pragma once
#include <glm/glm.hpp>
#include <string>
#include "shader_m.h"
class Light {
public:
	glm::vec3 position = glm::vec3(0.0f, 30.f, 0.0f);

	glm::vec3 ambient = glm::vec3(1, 1, 1) * 0.5f;
	glm::vec3 diffuse = glm::vec3(1,1,1) * 1.f;
	glm::vec3 specular = glm::vec3(1, 1, 1) * 1.f;

};