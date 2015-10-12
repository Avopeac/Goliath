#pragma once
#include <vector>
#include <GLM\glm.hpp>
#include <ASSIMP\Importer.hpp>
#include <ASSIMP\scene.h>
#include <ASSIMP\postprocess.h>
#include "..\Application.h"
#include "..\Model\Mesh.h"
#include "..\Model\Texture.h"
///Model is a mesh with a single attribute index, this code is derived from a tutorial.
class Model {
public:
	//Implements the drawable interface, calls base class constructor
	Model(GLchar* &path) { load_model(path); }
	//Overrides the draw function
	void Draw(Shader &shader);
	void DrawWireframe(Shader &shader);

private:
	//The meshes of the model
	std::vector<Mesh> _meshes;
	//The directory of the model assets
	std::string _directory;
	//How many textures are loaded
	std::vector<Texture> _textures_loaded;
	//Loads model from the given file path
	void load_model(const std::string &path);
	//Process a assimp node
	void process_node(aiNode* node, const aiScene* scene);
	//Process an assimp mesh
	Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
	//Load diffuse or specular textures
	std::vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type, const std::string &type_name);
};