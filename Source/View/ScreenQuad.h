#pragma once
#include <GLM\glm.hpp>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "..\Model\Vertex.h"
class ScreenQuad {
public:
	ScreenQuad() {
		glGenVertexArrays(1, &_VAO);
		glGenBuffers(1, &_VBO);
		glGenBuffers(1, &_EBO);
		glBindVertexArray(_VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), &_indices[0], GL_STATIC_DRAW);
		glBindVertexArray(_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, _VBO);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::texcoord));
	}

	void draw() const {
		glBindVertexArray(_VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	};

private:
	GLuint _VAO, _VBO, _EBO;
	Vertex _vertices[4]{
		{ { -1, -1, 0 }, { 0, 0, 0 }, { 0, 0 } },
		{ { 1, -1, 0 }, { 0, 0, 0 }, { 1, 0 } },
		{ { -1, 1, 0 }, { 0, 0, 0 }, { 0, 1 } },
		{ { 1, 1, 0 }, { 0, 0, 0 }, { 1, 1 } },
	};
	GLuint _indices[6]{ 0, 1, 2, 2, 1, 3 };
};