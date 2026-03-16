#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{  
	// luz ambiental
	float ambientStregth = 0.1;
	vec3 ambient = ambientStregth * lightColor;
	
	// luz difusa
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos); // Tenemos la direccion de la luz apuntando el fragmento, normalizando y restando  la posicion de la luz.

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// luz especular
	float specularStregth = 0.5;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec =  pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStregth * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * texture(texture1, TexCoords).rgb;
	FragColor = vec4(result, 1.0);
}