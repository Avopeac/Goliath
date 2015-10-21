#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL\glew.h>
#include <GL\GL.h>
#include <GLFW\glfw3.h>
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
	//Try openining and streaming in the shader files
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
		std::cout << "Application failed to read shader file. " << std::endl;
	}
	//Send the read shader code to compilation step
	compile_shader(vertex_code.c_str(), fragment_code.c_str());
}

void Shader::compile_shader(const char *vertex_code, const char *fragment_code) {
	GLuint vertex, fragment;
	GLint success;
	GLchar *log = new GLchar[_log_size];
	//Create vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_code, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, _log_size, NULL, log);
		std::cout << "Application failed to compile vertex shader. " << std::endl;
	}
	//Create fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_code, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, _log_size, NULL, log);
		std::cout << "Application failed to compile fragment shader. " << std::endl;
	}
	//Linked the shaders and create program
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, _log_size, NULL, log);
		std::cout << "Application failed to link shaders and create program. " << std::endl;
	}
	//Clean up
	delete log;
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() {
	if (_last_program != program) {
		glUseProgram(program);
		_last_program = program;
	}
}


