#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;

	sampler2D texture1;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform Light light;

uniform vec3 viewPos;

void main()
{  
	// luz ambiental
	vec3 ambient = light.ambient * material.ambient;
	
	// luz difusa
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos); // Tenemos la direccion de la luz apuntando el fragmento, normalizando y restando  la posicion de la luz.

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// luz especular

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec =  pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	vec3 result = (ambient + diffuse + specular) * texture(material.texture1, TexCoords).rgb;
	FragColor = vec4(result, 1.0);
}