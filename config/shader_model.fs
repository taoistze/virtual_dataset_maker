#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
uniform vec3 light_position=vec3(0.0f,0.0f,0.f);
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Material material;
uniform Light light;

in vec2 TexCoords;
uniform sampler2D texture_diffuse1;

void main()
{
    // diffuse 漫反射，与颜色有关的信息仅存储在漫反射中，因此只考虑漫反射
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	if (dot(norm, lightDir)<0.f)
		norm=-norm;
	float diff = dot(norm, lightDir);
	vec3 diffuse = diff * light.diffuse* ( material.diffuse);
	//调节一下使得绿、蓝没那么突出
	if (diffuse[1]>0.4f)//green
	{
		if (diffuse[0]<0.4)
		{
			diffuse[0]=0.13f;
			diffuse[1]=0.26f;
			diffuse[2]=0.17f;
			
		}
	}
	if (diffuse[2]>0.5f)//blue
	{
		if (diffuse[0]<0.5)
		{
			diffuse[0]=0.08f;
			diffuse[1]=0.08f;
			diffuse[2]=0.16f;
		}
	}
	if (diffuse[0]>0.5)//white
	{
		if (diffuse[1]>0.5)
		{
			if (diffuse[2]>0.5)
				diffuse=diffuse*1.0f;
		}
	}
	if (diffuse[0]>0.5)//red
	{
		if (diffuse[1]<0.5)
		{
			diffuse[0]=0.54f;
			diffuse[1]=0.12f;
			diffuse[2]=0.12f;
		}
	}
	
		

	FragColor = vec4(diffuse, 1.0f);
}