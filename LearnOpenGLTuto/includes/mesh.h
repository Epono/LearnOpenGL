#pragma once

#include <shader.h>

#include <string>
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct Texture {
	unsigned int ID;
	std::string name;
	std::string path;
};

class Mesh
{
public:
	// Mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);
	void Draw(Shader shader) const;

private:
	// Render data
	unsigned int VAO, VBO, EBO;

	void setupMesh();
};

