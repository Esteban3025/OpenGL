#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D texture1;
uniform vec3 lightColor;

void main() {
	vec3 objectColor = texture(texture1, texCoord).rgb;
	float ambientStregth = 0.1;
	vec3 ambient = ambientStregth * lightColor;


	vec3 result = ambient * objectColor;

	FragColor = vec4(result, 1.0f);
}