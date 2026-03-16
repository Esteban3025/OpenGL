#version 330 core
out vec4 FragColor;

in vec2 texCoord;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	sampler2D texture1;
};

uniform Material material;

void main() {
	vec3 ambient = material.ambient * texture(material.texture1, texCoord).rgb;

	FragColor = vec4(ambient, 1.0f);
}