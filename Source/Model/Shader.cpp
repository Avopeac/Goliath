#include "Shader.h"
#include <GL\glew.h>
#include <GL\GL.h>
#include <GLFW\glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vss, fss;
		
		vss << vShaderFile.rdbuf();
		fss << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vss.str();
		fragmentCode = fss.str();
	}
	catch(std::ifstream::failure e){
		std::cout << "ERROR::SHADER::FAILED_TO_READ_FILE" << std::endl;
	}

	const GLchar *vShaderCode = vertexCode.c_str();
	const GLchar *fShaderCode = fragmentCode.c_str();

	this->compileShader(vShaderCode, fShaderCode);
}

unsigned const int Shader::LOG_SIZE = 512;

void Shader::compileShader(const char *vShaderCode, const char *fShaderCode)
{
	GLuint vertex, fragment;
	GLint success;
	GLchar log[LOG_SIZE];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertex, LOG_SIZE, NULL, log);
		std::cout << "ERROR::SHADER::FAILED_TO_COMPILE_VERTEX_SHADER" << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragment, LOG_SIZE, NULL, log);
		std::cout << "ERROR::SHADER::FAILED_TO_COMPILE_FRAGMENT_SHADER" << std::endl;
	}

	this->program = glCreateProgram();
	glAttachShader(this->program, vertex);
	glAttachShader(this->program, fragment);
	glLinkProgram(this->program);
	glGetProgramiv(this->program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(this->program, LOG_SIZE, NULL, log);
		std::cout << "ERROR::SHADER::FAILED_TO_LINK_SHADER_PROGRAM" << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::Use()
{
	glUseProgram(this->program);
}


