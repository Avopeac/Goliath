#pragma once
#include <GL\glew.h>
#include <GL\gl.h>
#include <GLFW\glfw3.h>
///A shader is used for rendering mesh and model objects
class Shader
{
public:
	Shader(const GLchar *vertex_path, const GLchar *fragment_path);
	Shader(const GLchar *vertex_path, const GLchar *fragment_path, const GLchar *geometry_path, const GLchar *tess_ctrl_path, const GLchar *tess_eval_path);
	~Shader() { };
	void use() const;
	GLuint program;

private:
	unsigned static int _log_size;
	static GLuint _last_program;
	void compile_shader(const GLchar *, const GLchar *, const char *vertex_code, const char *fragment_code);
	void Shader::compile_shader(const GLchar *vert, const GLchar *frag, const GLchar *geom, const GLchar *tess_c, const GLchar *tess_e, const char *vertex_code, const char *fragment_code, const char *geometry_code, const char *tess_ctrl_code, const char *tess_eval_code);
};