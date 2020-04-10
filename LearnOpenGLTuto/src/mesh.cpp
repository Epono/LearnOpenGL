#include <mesh.h>

#include <glad/glad.h>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Normal
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	// TexCoords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	// Tangent
	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	// Bitangent
	//glEnableVertexAttribArray(4);
	//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader shader) const {
	unsigned int diffuseNumber = 0;
	unsigned int specularNumber = 0;
	unsigned int normalNumber = 0;
	unsigned int heightNumber = 0;

	for (unsigned int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		const Texture& texture = textures[i];
		std::string number;

		if (texture.name == "diffuse") {
			number = std::to_string(diffuseNumber);
			diffuseNumber++;
		}
		else if (texture.name == "specular") {
			number = std::to_string(specularNumber);
			specularNumber++;
		}
		else if (texture.name == "normal") {
 			number = std::to_string(normalNumber);
			normalNumber++;
			continue;
		}
		else if (texture.name == "height") {
			number = std::to_string(heightNumber);
			heightNumber++;
			continue;
		}
		// TODO: else

		std::string maisnon = ("material." + texture.name + (number == "0" ? "" : number));
		//std::cout << maisnon << std::endl;
		shader.setInt(maisnon.c_str(), i);
		shader.setFloat("material.shininess", 16.0f);
		glBindTexture(GL_TEXTURE_2D, texture.ID);
	}

 	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}