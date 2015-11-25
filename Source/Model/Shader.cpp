#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

unsigned int Shader::_log_size = 512;
GLuint Shader::_last_program = 0;

Shader::Shader(const GLchar * vertex_path, const GLchar * fragment_path) {
	std::string vertex_code;
	std::string fragment_code;
	std::ifstream vertex_file;
	std::ifstream fragment_file;
	vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragment_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	//Try opening and streaming in the shader files
	try {
		vertex_file.open(vertex_path);
		fragment_file.open(fragment_path);
		std::stringstream vss, fss;
		vss << vertex_file.rdbuf();
		fss << fragment_file.rdbuf();
		vertex_file.close();
		fragment_file.close();
		vertex_code = vss.str();
		fragment_code = fss.str();
	}
	catch (std::ifstream::failure e) {
		std::cerr << "Application failed to read shader file. " << std::endl;
	}
	//Send the read shader code to compilation step
	compile_shader(vertex_path, fragment_path, vertex_code.c_str(), fragment_code.c_str());
}

Shader::Shader(const GLchar* vertex_path, const GLchar* fragment_path, const GLchar *geometry_path, const GLchar* tess_ctrl_path, const GLchar* tess_eval_path) {
	std::string vertex_code;
	std::string fragment_code;
	std::string geometry_code;
	std::string tess_ctrl_code;
	std::string tess_eval_code;
	std::ifstream vertex_file;
	std::ifstream fragment_file;
	std::ifstream geometry_file;
	std::ifstream tess_ctrl_file;
	std::ifstream tess_eval_file;
	vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragment_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	geometry_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	tess_ctrl_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	tess_eval_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	//Try opening and streaming in the shader files
	try {
		vertex_file.open(vertex_path);
		fragment_file.open(fragment_path);
		geometry_file.open(geometry_path);
		tess_ctrl_file.open(tess_ctrl_path);
		tess_eval_file.open(tess_eval_path);
		std::stringstream vss, fss, gss, tcss, tess;
		vss << vertex_file.rdbuf();
		fss << fragment_file.rdbuf();
		gss << geometry_file.rdbuf();
		tcss << tess_ctrl_file.rdbuf();
		tess << tess_eval_file.rdbuf();
		vertex_file.close();
		fragment_file.close();
		geometry_file.close();
		tess_ctrl_file.close();
		tess_eval_file.close();
		vertex_code = vss.str();
		fragment_code = fss.str();
		geometry_code = gss.str();
		tess_ctrl_code = tcss.str();
		tess_eval_code = tess.str();
	}
	catch (std::ifstream::failure e) {
		std::cerr << "Application failed to read shader file. " << std::endl;
	}
	//Send the read shader code to compilation step
	compile_shader(vertex_path, fragment_path,  geometry_path, tess_ctrl_path, tess_eval_path, vertex_code.c_str(), fragment_code.c_str(), geometry_code.c_str(), tess_ctrl_code.c_str(), tess_eval_code.c_str());
}

void Shader::compile_shader(const GLchar *vert, const GLchar *frag, const char *vertex_code, const char *fragment_code) {
	compile_shader(vert, frag, nullptr, nullptr, nullptr, vertex_code, fragment_code, nullptr, nullptr, nullptr);
}

void Shader::compile_shader(const GLchar *vert, const GLchar *frag, const GLchar *geom, const GLchar *tess_c, const GLchar *tess_e, const char *vertex_code, const char *fragment_code, const char *geometry_code, const char *tess_ctrl_code, const char *tess_eval_code) {
	GLuint vertex, fragment, geometry, tess_ctrl, tess_eval;
	GLint success;
	GLchar *log = new GLchar[_log_size];
	//Create vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_code, nullptr);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, _log_size, nullptr, log);
		std::cerr << "Application failed to compile vertex shader with name \"" << vert << "\". " << std::endl;
	}
	//Create fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_code, nullptr);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, _log_size, nullptr, log);
		std::cerr << "Application failed to compile fragment shader with name \"" << frag << "\". " << std::endl;
	}
	//Create tessellation control shader
	if (geometry_code != nullptr) {
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &geometry_code, nullptr);
		glCompileShader(geometry);
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometry, _log_size, nullptr, log);
			std::cerr << "Application failed to compile geometry shader with name \"" << geom << "\". " << std::endl;
		}
	}
	//Create tessellation control shader
	if (tess_ctrl_code != nullptr) {
		tess_ctrl = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tess_ctrl, 1, &tess_ctrl_code, nullptr);
		glCompileShader(tess_ctrl);
		glGetShaderiv(tess_ctrl, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(tess_ctrl, _log_size, nullptr, log);
			std::cerr << "Application failed to compile tessellation control shader with name \"" << tess_c << "\". " << std::endl;
		}
	}
	//Create tessellation evaluation shader
	if (tess_eval_code != nullptr) {
		tess_eval = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(tess_eval, 1, &tess_eval_code, nullptr);
		glCompileShader(tess_eval);
		glGetShaderiv(tess_eval, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(tess_eval, _log_size, nullptr, log);
			std::cerr << "Application failed to compile tessellation evaluation shader with name \"" << tess_e << "\". " << std::endl;
		}
	}
	//Linked the shaders and create program
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	if (geometry_code != nullptr) {
		glAttachShader(program, geometry);
	}
	if (tess_ctrl_code != nullptr) {
		glAttachShader(program, tess_ctrl);
	}
	if (tess_eval_code != nullptr) {
		glAttachShader(program, tess_eval);
	}
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, _log_size, nullptr, log);
		std::cerr << "Application failed to link shaders and create program. " << std::endl;
	}
	//Clean up
	delete log;
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometry_code != nullptr) {
		glDeleteShader(geometry);
	}
	if (tess_ctrl_code != nullptr) {
		glDeleteShader(tess_ctrl);
	}
	if (tess_eval_code != nullptr) {
		glDeleteShader(tess_eval);
	}
}

void Shader::use() const {
	if (_last_program != program) {
		glUseProgram(program);
		_last_program = program;
	}
}


