#version 330 core
out vec4 FragColor; 

in vec3 OurColor;
in vec2 TexCoord;
in vec3 FragNormal;
in vec3 FragPosition;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float mixValue;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 viewPosition;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform int shininess;

void main()
{
	vec4 objectColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), mixValue);

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