#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 localPos;
uniform mat4 model;
uniform mat4 projection;
void main()
{
	vec4 targetCoord =  vec4(aPos, 1.0);
	localPos = targetCoord.xyz;
	vec4 camCoord =  targetCoord;
	gl_Position = projection *model* camCoord;
}	