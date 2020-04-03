#pragma once

#include <iostream>
#include <glm/glm.hpp>

class PointLight
{
private:
	static constexpr glm::vec3 DEFAULT_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
	static constexpr float DEFAULT_CONSTANT = 1.0f;
	static constexpr float DEFAULT_LINEAR = 0.09f;
	static constexpr float DEFAULT_QUADRATIC = 0.032f;
	static constexpr glm::vec3 DEFAULT_AMBIENT = glm::vec3(0.05f, 0.05f, 0.05f);
	static constexpr glm::vec3 DEFAULT_DIFFUSE = glm::vec3(0.8f, 0.8f, 0.8f);
	static constexpr glm::vec3 DEFAULT_SPECULAR = glm::vec3(1.0f, 1.0f, 1.0f);

public:
	glm::vec3 Position;

	float Constant;
	float Linear;
	float Quadratic;

	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;

	bool Enabled;
	bool Visible;

	PointLight(glm::vec3 position = DEFAULT_POSITION,
		float constant = DEFAULT_CONSTANT,
		float linear = DEFAULT_LINEAR,
		float quadratic = DEFAULT_QUADRATIC,
		glm::vec3 ambient = DEFAULT_AMBIENT,
		glm::vec3 diffuse = DEFAULT_DIFFUSE,
		glm::vec3 specular = DEFAULT_SPECULAR,
		bool enabled = true,
		bool visible = true) {

		Position = position;

		Constant = constant;
		Linear = linear;
		Quadratic = quadratic;

		Ambient = ambient;
		Diffuse = diffuse;
		Specular = specular;

		Enabled = enabled;
		Visible = visible;
	}
};