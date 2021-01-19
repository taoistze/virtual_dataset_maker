#pragma once
#include <glm/glm.hpp>
#include <assimp/material.h>
#include "shader_m.h"
class Material {
public:
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 ambient;
	float shininess = 1;

	void Set(const Shader& shader) const {
		shader.setVec3("material.diffuse", diffuse);
		if ((diffuse[0]<0)|(diffuse[1]<0)|(diffuse[2]<0))
			cout<<diffuse[0]<<" "<<diffuse[1]<<" "<<diffuse[2]<<endl;
		shader.setVec3("material.specular", specular);
		if ((specular[0] < 0) | (specular[1] < 0) | (specular[2] < 0))
			cout << specular[0] << " " << specular[1] << " " << specular[2] << endl;
		shader.setVec3("material.ambient", ambient);
		if ((ambient[0] < 0) | (ambient[1] < 0) | (ambient[2] < 0))
			cout << ambient[0] << " " << ambient[1] << " " << ambient[2] << endl;
		shader.setFloat("material.shininess", shininess);
	}
};
