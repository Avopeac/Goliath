#include "Tile.h"
#include <GLM/gtc/noise.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include "View/Renderer.h"
#include <Terrain\Noise3D.h>

Tile::Tile(unsigned int resolution, const glm::mat4 &scale, const glm::mat4 &translation, const glm::mat4 &rotation)
	: Drawable(), _resolution(resolution), _translation(translation), _scale(scale), _rotation(rotation), _inverse_resolution(1.0f / _resolution) {
	_premult_transf = _translation * _rotation * _scale;
}

void Tile::generate_vertex(glm::vec3 position) {
	Vertex vertex;
	vertex.position = glm::vec3(_translation * _scale * glm::vec4(position, 1.0));

	//vertex.position = glm::vec3(_premult_transf * glm::vec4(position, 1.0));
	float height;
	glm::vec3 derivative;
	Noise3D::get_noise(vertex.position, height, derivative);
	vertex.position.y += height;
	vertex.normal = glm::vec3(-derivative.x, 1.0, -derivative.z);

	//vertex.normal = glm::vec3(_rotation * glm::vec4(0, 1, 0, 1));
	vertex.texcoord = { position.x, position.z };
	_mesh.vertices.push_back(vertex);
}

void Tile::generate_skirt_vertex(glm::vec3 position) {
	Vertex vertex;
	//vertex.position = glm::vec3(_premult_transf * glm::vec4(position, 1.0));
	vertex.position = glm::vec3(_translation * _scale * glm::vec4(position, 1.0));
	vertex.position.y = _skirt_offset;

	//vertex.normal = glm::vec3(_rotation * glm::vec4(0, 1, 0, 1));
	vertex.normal = glm::vec3(0, 0, 0);
	vertex.texcoord = { position.x, position.z };
	_mesh.vertices.push_back(vertex);
}

void Tile::generate_vertex_helper(unsigned int column, bool edge) {
	float x = column * _inverse_resolution - offset, z = 0.0f;
	generate_skirt_vertex({ x, _skirt_offset, -offset });
	for (unsigned int row = 0; row <= _resolution; ++row) {
		z = row * _inverse_resolution - offset;
		edge ? generate_skirt_vertex({ x, _skirt_offset, z }) : generate_vertex({ x, 0, z });
	}
	generate_skirt_vertex({ x, _skirt_offset, _resolution * _inverse_resolution - offset });
}

void Tile::generate_normalized_mesh() {
	//Set up vertices
	generate_normalized_vertex_helper(0, true);
	for (unsigned int column = 0; column <= _resolution; ++column) {
		generate_normalized_vertex_helper(column, false);
	}
	generate_normalized_vertex_helper(_resolution, true);
	//Set up indices
	unsigned int stride = _resolution + _skirt_padding + 1;
	for (unsigned int i = 0; i < _resolution + _skirt_padding; ++i) {
		for (unsigned int j = 0; j < _resolution + _skirt_padding; ++j) {
			_mesh.indices.push_back(i + 1 + j * stride);
			_mesh.indices.push_back(i + (j + 1) * stride);
			_mesh.indices.push_back(i + j * stride);
			_mesh.indices.push_back(i + 1 + (j + 1) * stride);
			_mesh.indices.push_back(i + (j + 1) * stride);
			_mesh.indices.push_back(i + 1 + j * stride);
		}
	}
	//Upload mesh
	_mesh.setup_mesh();
}

void Tile::generate_normalized_vertex(glm::vec3 position) {
	glm::vec3 sphere_pos = glm::normalize(glm::vec3(_premult_transf * glm::vec4(position, 1))); //Normalized position
	float height;
	glm::vec3 gradient;
	float heightacc = 0;
	glm::vec3 gradientacc;

	Noise3D::get_noise(sphere_pos, height, gradient); //Get noise and it's derivatives
	heightacc += 0.5f * height;
	gradientacc += gradient;
	gradient = gradientacc;
	height = heightacc;
	glm::vec3 tangent_point = gradient - glm::dot(glm::normalize(gradient), glm::normalize(sphere_pos)) * sphere_pos; //Project gradient onto sphere surface
	Vertex vertex;
	vertex.position = height * sphere_pos + glm::vec3(4.0f * glm::vec4(sphere_pos, 1));
	vertex.normal = sphere_pos - height * tangent_point; //Scale it with radius
	vertex.texcoord = { position.x, position.z };
	_mesh.vertices.push_back(vertex);
}

void Tile::generate_normalized_skirt_vertex(glm::vec3 position) {
	Vertex vertex;
	glm::vec3 temp_pos = glm::vec3(_premult_transf * glm::vec4(position, 1));
	vertex.position = 0.8f * glm::vec3(4.0f * glm::normalize(glm::vec4(temp_pos, 1)));
	vertex.normal = glm::vec3(0, 0, 0);
	vertex.texcoord = { position.x, position.z };
	_mesh.vertices.push_back(vertex);
}

void Tile::generate_normalized_vertex_helper(unsigned int column, bool edge) {
	float x = column * _inverse_resolution - offset, z = 0.0f;
	generate_normalized_skirt_vertex({ x, _skirt_offset, -offset });
	for (unsigned int row = 0; row <= _resolution; ++row) {
		z = row * _inverse_resolution - offset;
		edge ? generate_normalized_skirt_vertex({ x, _skirt_offset, z }) : generate_normalized_vertex({ x, 0, z });
	}
	generate_normalized_skirt_vertex({ x, _skirt_offset, _resolution * _inverse_resolution - offset });
}

void Tile::generate_mesh() {
	//Set up vertices
	generate_vertex_helper(0, true);
	for (unsigned int column = 0; column <= _resolution; ++column) {
		generate_vertex_helper(column, false);
	}
	generate_vertex_helper(_resolution, true);
	//Set up indices
	unsigned int stride = _resolution + _skirt_padding + 1;
	for (unsigned int i = 0; i < _resolution + _skirt_padding; ++i) {
		for (unsigned int j = 0; j < _resolution + _skirt_padding; ++j) {
			_mesh.indices.push_back(i + 1 + j * stride);
			_mesh.indices.push_back(i + (j + 1) * stride);
			_mesh.indices.push_back(i + j * stride);
			_mesh.indices.push_back(i + 1 + (j + 1) * stride);
			_mesh.indices.push_back(i + (j + 1) * stride);
			_mesh.indices.push_back(i + 1 + j * stride);
		}
	}

	//Upload mesh
	_mesh.setup_mesh();
}

void Tile::setup_draw(const Camera & camera, double delta_time) {
	_shader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
}

void Tile::draw(const Camera & camera, double delta_time) {
	setup_draw(camera, delta_time);
	_mesh.draw(_shader, delta_time);
}

void Tile::draw_wireframe(const Camera & camera, double delta_time) {
	setup_draw(camera, delta_time);
	_mesh.draw_wireframe(_shader, delta_time);
}
