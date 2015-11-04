#include "Tile.h"
#include <GLM/gtc/type_ptr.hpp>
#include "View/Renderer.h"

Tile::Tile(unsigned int resolution, const glm::mat4 &scale, const glm::mat4 &translation, const glm::mat4 &rotation, bool normalize)
	: Drawable(), _resolution(resolution), _translation(translation), _normalize(normalize), _scale(scale), _rotation(rotation) {
	set_shader(Renderer::instance().get_standard_shader(), true);
	generate_mesh();
}

void Tile::generate_mesh() {
	float step = 1.0f / _resolution;
	float offset = 0.5f;
	unsigned int i, j;
	float x, z;
	int counter = 0;
	for (i = 0; i <= _resolution; ++i) {
		for (j = 0; j <= _resolution; ++j) {
			x = i * step;
			z = j * step;
			Vertex vertex;
			glm::vec4 position(x - offset, 0, z - offset, 1.0);
			if (_normalize) {
				position = _translation * _rotation * _scale * position;
				glm::vec4 normal = glm::normalize(glm::vec4(position.x, position.y, position.z, 0.0));
				vertex.position = glm::vec3(_scale * normal);
				vertex.normal = { normal.x, normal.y, normal.z };
			}
			else {
				vertex.position = glm::vec3(_translation * _rotation * _scale * position);
				vertex.normal = glm::vec3(_rotation * glm::vec4(0, 1, 0, 0));
			}
			vertex.texcoord = { x, z };
			_mesh.vertices.push_back(vertex);
		}
	}

	unsigned int stride = _resolution + 1;
	for (i = 0; i < _resolution; ++i) {
		for (j = 0; j < _resolution; ++j) {
			_mesh.indices.push_back(i + 1 + j * stride);
			_mesh.indices.push_back(i + (j + 1) * stride);
			_mesh.indices.push_back(i + j * stride);
			_mesh.indices.push_back(i + 1 + (j + 1) * stride);
			_mesh.indices.push_back(i + (j + 1) * stride);
			_mesh.indices.push_back(i + 1 + j * stride);
		}
	}
	_mesh.setup_mesh();
}

void Tile::setup_draw(const Lighting &lighting, const Camera & camera, double delta_time) {
	_shader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	// I think _translation, _rotation, _scale is done in mesh generation.
	// Could probably be uploaded here instead for better performance ?
	glm::mat4 model;
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
}

void Tile::draw(const Lighting &lighting, const Camera & camera, double delta_time) {
	setup_draw(lighting, camera, delta_time);
	_mesh.draw_wireframe(_shader, delta_time);
}

void Tile::draw_wireframe(const Lighting &lighting, const Camera & camera, double delta_time) {
	setup_draw(lighting, camera, delta_time);
	_mesh.draw_wireframe(_shader, delta_time);
}
