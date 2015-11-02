#include "Mesh.h"
#include <sstream>
#include <iostream>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	setup_mesh();
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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	//Vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	//Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::normal));
	//Texcoords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::texcoord));
}

void Mesh::draw(std::shared_ptr<Shader> shader, double delta_time) {
	GLuint diffuse_nr = 1;
	GLuint specular_nr = 1;
	//Set textures to texture units
	for (GLuint i = 0; i < textures.size(); ++i) {
		std::stringstream ss;
		std::string number;
		std::string name = textures[i]._type;
		if (name == Texture::_diffuse_id) {
			ss << diffuse_nr++;
		}
		else if (name == Texture::_specular_id) {
			ss << specular_nr++;
		}
		number = ss.str();
		std::string uniform = name + number;
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]._id);
		glUniform1i(glGetUniformLocation(shader->program, uniform.c_str()), i);
	}
	//Reset active texture unit
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
