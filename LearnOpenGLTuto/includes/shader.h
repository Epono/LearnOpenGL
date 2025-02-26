#pragma once

#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath);

	void use();
	static void release();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;

	void setFloat3(const std::string& name, const glm::vec3& value) const;
	void setFloat3(const std::string& name, float r, float g, float b) const;

	void setFloat4(const std::string& name, const glm::vec4& value) const;
	void setFloat4(const std::string& name, float r, float g, float b, float a) const;

	void setMatrixFloat4v(const std::string& name, int count, const glm::mat4& mat) const;
};