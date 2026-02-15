#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;

void main() 
{
	vec3 diffuse = texture(texture_diffuse1, TexCoords).rgb; 
	vec3 diffuse1 = texture(texture_diffuse1, TexCoords).rgb; 
	vec3 specular = texture(texture_specular, TexCoords).rgb; 
	vec3 normal = texture(texture_normal, TexCoords).rgb; 
	
	vec3 result = diffuse * diffuse1;

	FragColor = vec4(diffuse, 1.0f);
};
