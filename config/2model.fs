#version 330 core
out vec4 FragColor;

in vec3 localPos;
void main()
{    
    FragColor = vec4(localPos,1.0f);
}