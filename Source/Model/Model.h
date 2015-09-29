#pragma once
#include "..\Application.h"
#include "..\Model\Mesh.h"
#include "..\Model\Texture.h"

#include <vector>
#include <GLM\glm.hpp>
#include <ASSIMP\Importer.hpp>
#include <ASSIMP\scene.h>
#include <ASSIMP\postprocess.h>

///Model is a mesh with a single attribute index, this code is derived from a tutorial.
///Author: Andreas Larsson
///Date: 2015-08-19
class Model {
public:

	//Implements the drawable interface, calls base class constructor
	Model(GLchar* &path) {
		this->LoadModel(path);
	}

	//Overrides the draw function
	void Draw(Shader &shader);
	void DrawWireframe(Shader &shader);

private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> texturesLoaded;

	void LoadModel(const std::string &path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);

};