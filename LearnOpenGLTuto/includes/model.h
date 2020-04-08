#pragma once

#include <shader.h>
#include <mesh.h>

#include <string>
#include <vector>

// https://github.com/assimp/assimp/issues/1566
// https://github.com/assimp/assimp/issues/583
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
	Model(const std::string& path);
	void Draw(const Shader& shader);

private:
	std::vector<Mesh> meshes;
	std::string directory;

	std::vector<Texture> texturesLoaded;

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
};

