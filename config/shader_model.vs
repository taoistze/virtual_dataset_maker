#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 projection;

void main()
{ 
    gl_Position = projection * model * vec4(aPos, 1.0);
	FragPos=aPos;
	Normal=aNormal;
}