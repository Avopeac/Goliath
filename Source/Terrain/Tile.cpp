#include "Tile.h"
#include <GLM/gtc/type_ptr.hpp>
#include "View/Renderer.h"

Tile::Tile(unsigned int resolution, const glm::mat4 &scale, const glm::mat4 &translation, const glm::mat4 &rotation)
	: Drawable(), _resolution(resolution), _translation(translation), _scale(scale), _rotation(rotation) {
	set_shader(Renderer::instance().get_standard_shader(), true);
	_premult_transf = _translation * _rotation * _scale;
	//Do this last!
	generate_mesh();
}

void Tile::generate_vertex(glm::vec3 position) {
	Vertex vertex;
	vertex.position = glm::vec3(_premult_transf * glm::vec4(position, 1.0));
	vertex.normal = glm::vec3(_rotation * glm::vec4(0, 1, 0, 1));
	vertex.texcoord = { position.x, position.z };
	_mesh.vertices.push_back(vertex);
}

void Tile::generate_vertex_helper(float offset, float step, unsigned int column, bool edge) {
	float x = column * step - offset, z = 0.0f;
	generate_vertex({ x, _skirt_offset, -offset });
	for (unsigned int row = 0; row <= _resolution; ++row) {
		z = row * step - offset;
		edge ? generate_vertex({ x, _skirt_offset, z }) : generate_vertex({ x, 0, z });
	}
	generate_vertex({ x, _skirt_offset, _resolution * step - offset });
}

void Tile::generate_mesh() {
	//Set up vertices
	float step = 1.0f / _resolution;
	float offset = 0.5f;
	generate_vertex_helper(offset, step, 0, true);
	for (unsigned int column = 0; column <= _resolution; ++column) {
		generate_vertex_helper(offset, step, column, false);
	}
	generate_vertex_helper(offset, step, _resolution, true);
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

void Tile::setup_draw(const Lighting &lighting, const Camera & camera, double delta_time) {
	_shader->use();
	glDisable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glUniform3fv(glGetUniformLocation(_shader->program, "albedo"), 1, glm::value_ptr(_material.albedo));
	glUniform1f(glGetUniformLocation(_shader->program, "roughness"), _material.roughness);
	glUniform1f(glGetUniformLocation(_shader->program, "gaussian"), _material.gaussian);
	glUniform1f(glGetUniformLocation(_shader->program, "absorption"), _material.absorption);
	glUniform1f(glGetUniformLocation(_shader->program, "refraction"), _material.refraction);
	glm::mat4 model;
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	//These are set in renderer if using standard shader!
	//glUniformMatrix4fv(glGetUniformLocation(_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	//glUniformMatrix4fv(glGetUniformLocation(_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
}

void Tile::draw(const Lighting &lighting, const Camera & camera, double delta_time) {
	setup_draw(lighting, camera, delta_time);
	_mesh.draw(_shader, delta_time);
}

void Tile::draw_wireframe(const Lighting &lighting, const Camera & camera, double delta_time) {
	setup_draw(lighting, camera, delta_time);
	_mesh.draw_wireframe(_shader, delta_time);
}
