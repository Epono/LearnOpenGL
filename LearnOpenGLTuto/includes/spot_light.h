#pragma once

#include <iostream>
#include <glm/glm.hpp>

class SpotLight
{
private:
	static constexpr glm::vec3 DEFAULT_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
	static constexpr glm::vec3 DEFAULT_DIRECTION = glm::vec3(0.0f, 0.0f, -1.0f);
	static inline const float DEFAULT_INNER_CUT_OFF = glm::cos(glm::radians(12.5f));
	static inline const float DEFAULT_OUTER_CUT_OFF = glm::cos(glm::radians(15.0f));
	static constexpr float DEFAULT_CONSTANT = 1.0f;
	static constexpr float DEFAULT_LINEAR = 0.09f;
	static constexpr float DEFAULT_QUADRATIC = 0.032f;
	static constexpr glm::vec3 DEFAULT_AMBIENT = glm::vec3(0.0f, 0.0f, 0.0f);
	static constexpr glm::vec3 DEFAULT_DIFFUSE = glm::vec3(1.0f, 1.0f, 1.0f);
	static constexpr glm::vec3 DEFAULT_SPECULAR = glm::vec3(1.0f, 1.0f, 1.0f);

public:
	glm::vec3 Position;
	glm::vec3 Direction;

	float InnerCutOff;
	float OuterCutOff;

	float Constant;
	float Linear;
	float Quadratic;

	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;

	bool Enabled;
	bool Visible;

	SpotLight(glm::vec3 position = DEFAULT_POSITION,
		glm::vec3 direction = DEFAULT_DIRECTION,
		float innerCutOff = DEFAULT_INNER_CUT_OFF,
		float outerCutOff = DEFAULT_OUTER_CUT_OFF,
		float constant = DEFAULT_CONSTANT,
		float linear = DEFAULT_LINEAR,
		float quadratic = DEFAULT_QUADRATIC,
		glm::vec3 ambient = DEFAULT_AMBIENT,
		glm::vec3 diffuse = DEFAULT_DIFFUSE,
		glm::vec3 specular = DEFAULT_SPECULAR,
		bool enabled = true,
		bool visible = true) {

		Position = position;
		Direction = direction;

		InnerCutOff = innerCutOff;
		OuterCutOff = outerCutOff;

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