#pragma once
#include <string>
#include <assimp\postprocess.h>
#include <GLFW\glfw3.h>
#ifndef DIFFUSE_ID
#define DIFFUSE_ID "tex_diffuse"
#endif
#ifndef SPECULAR_ID
#define SPECULAR_ID "tex_specular"
#endif
///A texture has either a diffuse identifier or a specular identifier. This is set when loading models from asset import.
struct Texture {
	static const std::string _diffuse_id;
	static const std::string _specular_id;
	GLuint _id;
	std::string _type;
	aiString _path;
};