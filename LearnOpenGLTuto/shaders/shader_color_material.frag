#version 330 core
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 FragNormal;
in vec3 FragPosition;

out vec4 FragColor;

uniform vec3 viewPosition;
uniform Material material;
uniform Light light;

void main()
{
	//// Phong
	// Ambient
	vec3 ambient = light.ambient * material.ambient;

	// Diffuse
	vec3 fragNormal = normalize(FragNormal);							// norm
	vec3 lightDirection = normalize(light.position - FragPosition);
	float diff = max(dot(fragNormal, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// Specular
	vec3 viewDirection = normalize(viewPosition - FragPosition);
	vec3 reflectDirection = reflect(-lightDirection, fragNormal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0f);
}