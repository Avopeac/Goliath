#pragma once
#include <string>
#include <assimp\postprocess.h>
#include "..\Application.h"
///A texture has either a diffuse identifier or a specular identifier. This is set when loading models from asset import.
struct Texture {
	static const std::string _diffuse_identifier;
	static const std::string _specular_identifier;
	GLuint _id;
	std::string _type;
	aiString _path;
};