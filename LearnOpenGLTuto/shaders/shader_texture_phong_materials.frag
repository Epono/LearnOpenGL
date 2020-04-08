#version 330 core
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	sampler2D normal;
	sampler2D height;
	float shininess;
};

struct DirectionalLight {
	vec3 direction;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 position;
	vec3 direction;

	float innerCutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2		TexCoord;
in vec3		FragNormal;
in vec3		FragPosition;

out vec4	FragColor; 

uniform		vec3				viewPosition;
uniform		Material			material;

uniform		DirectionalLight	directionalLight;
uniform		PointLight			pointLights[10];	// TODO: pas besoin de spécifier de taille ?
uniform		SpotLight			spotLights[10];		// TODO: pas besoin de spécifier de taille ?

vec3 computeDirectionalLight(DirectionalLight light, vec3 fragNormal, vec3 viewDirection) {
	vec3 lightDirection = normalize(-light.direction);
	
	// Ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

	// Diffuse
	float diff = max(dot(fragNormal, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

	// Specular
	vec3 reflectDirection = reflect(-lightDirection, fragNormal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

	// Emission
	vec3 emission = vec3(texture(material.emission, TexCoord));

	// 
	return ambient + diffuse + specular + emission;
}

vec3 computePointLight(PointLight light, vec3 fragNormal, vec3 fragPosition, vec3 viewDirection) {
	vec3 lightDirection = normalize(light.position - fragPosition);

	float distance = length(light.position - fragPosition);
	float attenuation = 1 / (1 + light.constant + (light.linear * distance) + (light.quadratic * distance * distance));

	// Ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

	// Diffuse
	float diff = max(dot(fragNormal, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

	// Specular
	vec3 reflectDirection = reflect(-lightDirection, fragNormal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
	
	// Emission
	vec3 emission = vec3(texture(material.emission, TexCoord));
	
	// Attenuate
	ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    emission *= attenuation;

	return ambient + diffuse + specular + emission;
}

vec3 computeSpotLight(SpotLight light, vec3 fragNormal, vec3 fragPosition, vec3 viewDirection) {
	vec3 lightDirection = normalize(light.position - fragPosition);

	float distance = length(light.position - fragPosition);
	float attenuation = 1 / (1 + light.constant + (light.linear * distance) + (light.quadratic * distance * distance));

	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = light.innerCutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); // TODO: change ease function with bezier ?
	// float intensity = smoothstep(0.0, 1.0, (theta - light.outerCutOff) / epsilon); // TODO: check
	// TODO: "You can also use a "cookie" texture to simulate a real flashlight by multiplying it with the fragment color and the light attenuation. Google "flashlight texture"."
	// https://uploads.disquscdn.com/images/7e0feb070eb4b76a32179e2fa4ef26e40a340d9bb9c76d2449a8685d30fd7919.jpg
	// https://uploads.disquscdn.com/images/c917ceac2c0ab5583a33b6767d4e7c859268214b689bacf5ce6e960e4c54dca4.jpg

	// Ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

	// Diffuse
	float diff = max(dot(fragNormal, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

	// Specular
	vec3 reflectDirection = reflect(-lightDirection, fragNormal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
	
	// Emission
	vec3 emission = vec3(texture(material.emission, TexCoord));
	
	// Attenuate
	ambient  *= attenuation * intensity; // comment if there is no directional light to always have some light
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;
    emission *= attenuation * intensity;

	return ambient + diffuse + specular + emission;
}


// TODO: Lots of optimization (ambient + ?)
void main()
{
//	FragColor = texture(material.diffuse, TexCoord) + texture(material.specular, TexCoord);

	vec3 fragNormal = normalize(FragNormal);
	vec3 viewDirection = normalize(viewPosition - FragPosition);
	vec3 result = vec3(0.0, 0.0, 0.0);

	// DirectionalLight
	result += computeDirectionalLight(directionalLight, fragNormal, viewDirection);

	// PointLights
	for	(int i = 0; i < 10; ++i) {
		result += computePointLight(pointLights[i], fragNormal, FragPosition, viewDirection);
	}

	// SpotLights
	for	(int i = 0; i < 10; ++i) {
		result += computeSpotLight(spotLights[i], fragNormal, FragPosition, viewDirection);
	}

	FragColor = vec4(result, 1.0f);
}