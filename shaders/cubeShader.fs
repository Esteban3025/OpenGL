#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D specular;
    float shininess;
    sampler2D diffuse;
    sampler2D emission;
};
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;  

uniform Light light; 
uniform Material material;
uniform vec3 viewPos;
uniform vec3 emissionColor;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = light.diffuse  * diff * texture(material.diffuse, TexCoords).rgb; 
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    vec3 emission = texture(material.emission, TexCoords).rgb * emissionColor;
     
    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0);  
}
