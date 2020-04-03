#version 330 core
out vec4 FragColor;

in vec3 FragNormal;
in vec3 FragPosition;

uniform vec4 ourColor;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 viewPosition;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float shininess;

void main()
{
	vec4 objectColor = ourColor;

	//// Phong
	// Ambient
	vec3 ambient = ambientStrength * lightColor;

	// Diffuse
	vec3 fragNormal = normalize(FragNormal);							// norm
	vec3 lightDirection = normalize(lightPosition - FragPosition);
	float diff = max(dot(fragNormal, lightDirection), 0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor;

	// Specular
	vec3 viewDirection = normalize(viewPosition - FragPosition);
	vec3 reflectDirection = reflect(-lightDirection, fragNormal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor.rgb;
	FragColor = vec4(result, 1.0f);
}