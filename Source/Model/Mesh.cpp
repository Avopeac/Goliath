#include "Mesh.h"
#include <sstream>
#include <iostream>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices) {
	this->vertices = vertices;
	this->indices = indices;
}

void Mesh::setup_mesh() {
	//Generate buffers and bind buffer data
	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	glGenBuffers(1, &_EBO);
	update_indices();
	update_vertices();
}

void Mesh::update_indices() {
	//Re-upload buffer data
	glBindVertexArray(_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
}

void Mesh::update_vertices() {
	//Re-upload buffer data
	glBindVertexArray(_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(struct Vertex), &vertices[0], GL_STATIC_DRAW);
	//Vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);
	//Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)offsetof(Vertex, Vertex::normal));
	//Texcoords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)offsetof(Vertex, Vertex::texcoord));
	//Colors
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)offsetof(Vertex, Vertex::color));
	//Bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)offsetof(Vertex, Vertex::extra));

}

void Mesh::draw(std::shared_ptr<Shader> shader, double delta_time) {
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(_VAO);
	//Draw with index list
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::draw_wireframe(std::shared_ptr<Shader> shader, double delta_time) {
	//No regard to textures, just draw with index list
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(_VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
