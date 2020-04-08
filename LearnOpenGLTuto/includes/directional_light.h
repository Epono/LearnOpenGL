#pragma once

#include <iostream>
#include <glm/glm.hpp>

class DirectionalLight
{
private:
	static constexpr glm::vec3 DEFAULT_DIRECTION = glm::vec3(-0.2f, -1.0f, -0.3f);
	static constexpr glm::vec3 DEFAULT_AMBIENT = glm::vec3(0.05f, 0.05f, 0.05f);
	static constexpr glm::vec3 DEFAULT_DIFFUSE = glm::vec3(0.4f, 0.4f, 0.4f);
	static constexpr glm::vec3 DEFAULT_SPECULAR = glm::vec3(0.5f, 0.5f, 0.5f);

public:
	glm::vec3 Direction;

	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;

	bool Enabled;
	bool Visible;

	DirectionalLight(glm::vec3 direction = DEFAULT_DIRECTION,
		glm::vec3 ambient = DEFAULT_AMBIENT,
		glm::vec3 diffuse = DEFAULT_DIFFUSE,
		glm::vec3 specular = DEFAULT_SPECULAR,
		bool enabled = true,
		bool visible = true) {

		Direction = glm::normalize(direction);

		Ambient = ambient;
		Diffuse = diffuse;
		Specular = specular;

		Enabled = enabled;
		Visible = visible;
	}
};