#include "Tile.h"
#include "..\View\Renderer.h"
#include <GLM\gtc\matrix_transform.hpp>

Tile::Tile(unsigned int resolution, const glm::mat4 &scale, const glm::vec3 &translation, const glm::mat4 &rotation)
	: Drawable(), _resolution(resolution), _translation(translation) {
	set_shader(Renderer::instance().get_standard_shader(), true);
	_model = glm::mat3(scale * rotation);
	generate_mesh();
}

void Tile::generate_mesh() {
	float step = 1.0f / _resolution;
	float offset = 0.5f;
	unsigned int i, j;
	float x, z;
	for (i = 0; i <= _resolution; ++i) {
		for (j = 0; j <= _resolution; ++j) {
			x = i * step;
			z = j * step;
			Vertex vertex;
			vertex.position = _model * glm::vec3(x - offset, 0, z - offset) + _translation;
			vertex.normal = { 0, 0, 0 };
			vertex.texcoord = { x, z };
			_mesh.vertices.push_back(vertex);
		}
	}
	for (i = 0; i < _resolution; ++i) {
		for (j = 0; j < _resolution; ++j) {
			_mesh.indices.push_back(i + 1 + j * (_resolution + 1));
			_mesh.indices.push_back(i + (j + 1) * (_resolution + 1));
			_mesh.indices.push_back(i + j * (_resolution + 1));
			_mesh.indices.push_back(i + 1 + (j + 1) * (_resolution + 1));
			_mesh.indices.push_back(i + (j + 1) * (_resolution + 1));
			_mesh.indices.push_back(i + 1 + j * (_resolution + 1));
		}
	}
	_mesh.setup_mesh();
}

void Tile::draw(const Lighting &lighting, const Camera & camera, double delta_time) {
	draw_wireframe(lighting, camera, delta_time);
	//_shader->use();
	//glDisable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	//_mesh.draw(_shader, delta_time);
}

void Tile::draw_wireframe(const Lighting &lighting, const Camera & camera, double delta_time) {
	_shader->use();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	_mesh.draw_wireframe(_shader, delta_time);
}
