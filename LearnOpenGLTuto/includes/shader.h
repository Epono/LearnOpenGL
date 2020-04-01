#pragma once

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath);

	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setFloat4(const std::string& name, float r, float g, float b, float a) const;
	void setMatrixFloat4v(const std::string& name, int count, const glm::mat4& mat) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
};