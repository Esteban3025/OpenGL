#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D flashLight;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float outerCutOff;
	float cutOff;
	float constant;
	float linear;
	float quadratic;
};

uniform Material material;
uniform Light light;

uniform vec3 viewPos;

void main() {
	// luz ambiental
	// estoy usando la texture ya cargada del diffuse como ambient tambien 
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	// luz difusa
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos); // Tenemos la direccion de la luz apuntando el fragmento, normalizando y restando  la posicion de la luz.
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
		
	// luz especular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec =  pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// vec3 specular = light.specular * spec * vec3(texture(material.flashLight, TexCoords));

	// spotlight (bordes suaves)
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = smoothstep(0.0, 1.0, (theta - light.outerCutOff) / epsilon);
	// specular *= intensity;
	diffuse *= intensity;

	// atenuacion
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient *= attenuation;
	// specular *= attenuation;
	diffuse *= attenuation;

	vec3 result = (ambient + diffuse);
	FragColor = vec4(result, 1.0);
}