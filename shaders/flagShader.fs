#version 330 core
out vec4 FragColor;

uniform vec3 flagAndLightColor;

void main()
{
	FragColor = vec4(flagAndLightColor, 1.0f);
}