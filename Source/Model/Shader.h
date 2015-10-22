#pragma once
#include <GL\glew.h>
#include <GL\gl.h>
#include <GLFW\glfw3.h>
#ifndef STANDARD_VERT_SHADER
#define STANDARD_VERT_SHADER "Shaders/standard.vert"
#endif
#ifndef STANDARD_FRAG_SHADER
#define STANDARD_FRAG_SHADER "Shaders/standard.frag"
#endif
///A shader is used for rendering mesh and model objects
class Shader
{
public:
	Shader(const GLchar *vertex_path = STANDARD_VERT_SHADER, const GLchar *fragment_path = STANDARD_FRAG_SHADER);
	~Shader() { };
	void use() const;
	GLuint program;
private:
	unsigned static int _log_size;
	static GLuint _last_program;
	void compile_shader(const char *vertex_code, const char *fragment_code);
};