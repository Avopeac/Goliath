#pragma once
#include <GL\glew.h>
#include <GL\gl.h>
#include <GLFW\glfw3.h>

class Shader
{
public:
	
	Shader(const GLchar *, const GLchar *);
	~Shader() { };
	void Use();
	GLuint program;

private:

	static unsigned const int LOG_SIZE;
	void compileShader(const char *, const char *);
};