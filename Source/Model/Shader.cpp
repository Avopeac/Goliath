#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL\glew.h>
#include <GL\GL.h>
#include <GLFW\glfw3.h>
#include "Shader.h"

unsigned const int Shader::_log_size = 512;

Shader::Shader(const GLchar * vertex_path, const GLchar * fragment_path) {
	std::string vertex_code;
	std::string fragment_code;
	std::ifstream v_shader_file;
	std::ifstream f_shader_file;
	v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	//Try openining and streaming in the shader files
	try {
		v_shader_file.open(vertex_path);
		f_shader_file.open(fragment_path);
		std::stringstream vss, fss;
		vss << v_shader_file.rdbuf();
		fss << f_shader_file.rdbuf();
		v_shader_file.close();
		f_shader_file.close();
		vertex_code = vss.str();
		fragment_code = fss.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "Application failed to read shader file. " << std::endl;
	}
	const GLchar *v_shader_code = vertex_code.c_str();
	const GLchar *f_shader_code = fragment_code.c_str();
	//Send the read shader code to compilation step
	compile_shader(v_shader_code, f_shader_code);
}

void Shader::compile_shader(const char *v_shader_code, const char *f_shader_code) {
	GLuint vertex, fragment;
	GLint success;
	GLchar log[_log_size];
	//Create vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &v_shader_code, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, _log_size, NULL, log);
		std::cout << "Application failed to compile vertex shader. " << std::endl;
	}
	//Create fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &f_shader_code, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, _log_size, NULL, log);
		std::cout << "Application failed to compile fragment shader. " << std::endl;
	}
	//Linked the shaders and create program
	_program = glCreateProgram();
	glAttachShader(_program, vertex);
	glAttachShader(_program, fragment);
	glLinkProgram(_program);
	glGetProgramiv(_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->_program, _log_size, NULL, log);
		std::cout << "Application failed to link shaders and create program. " << std::endl;
	}
	//Clean up
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() {
	glUseProgram(this->_program);
}


