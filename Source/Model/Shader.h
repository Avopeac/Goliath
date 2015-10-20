#pragma once
#include <GL\glew.h>
#include <GL\gl.h>
#include <GLFW\glfw3.h>
///A shader is used for mesh and model objects
class Shader
{
public:
	Shader(const GLchar *, const GLchar *);
	~Shader() { };
	void use();
	GLuint _program;
private:
	unsigned const int _log_size = 512;
	void compile_shader(const char *, const char *);
};