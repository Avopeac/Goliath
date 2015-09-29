#pragma once
#include "..\Application.h"
#include "assimp\postprocess.h"
#include <string>

struct Texture
{
	static const std::string DIFFUSE_IDENTIFIER;
	static const std::string SPECULAR_IDENTIFIER;

	GLuint id;
	std::string type;
	aiString path;
};