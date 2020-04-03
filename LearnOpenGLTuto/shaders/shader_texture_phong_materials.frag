#version 330 core
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct Light {
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 TexCoord;
in vec3 FragNormal;
in vec3 FragPosition;

out vec4 FragColor; 

uniform vec3 viewPosition;
uniform Material material;
uniform Light light;
uniform float time;

void main()
{
	//// Phong
	// Ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

	// Diffuse
	vec3 fragNormal = normalize(FragNormal);							// norm
	vec3 lightDirection = normalize(light.position - FragPosition);
	float diff = max(dot(fragNormal, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

	// Specular
	vec3 viewDirection = normalize(viewPosition - FragPosition);
	vec3 reflectDirection = reflect(-lightDirection, fragNormal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

	// Emission
	vec3 emission = vec3(texture(material.emission, TexCoord));

	vec3 result = ambient + diffuse + specular + emission;
	FragColor = vec4(result, 1.0f);
}