#include "Tile.h"
#include <GLM/gtc/type_ptr.hpp>
#include "View/Renderer.h"

Tile::Tile(unsigned int resolution, const glm::mat4 &scale, const glm::mat4 &translation, const glm::mat4 &rotation, bool normalize)
	: Drawable(), _resolution(resolution), _translation(translation), _normalize(normalize), _scale(scale), _rotation(rotation) {
	set_shader(Renderer::instance().get_standard_shader(), true);
	generate_mesh();
}

void Tile::generate_vertex(glm::vec3 position) {
	Vertex vertex;
	vertex.position = glm::vec3(_scale * glm::vec4(position, 1));
	vertex.position = glm::vec3(_scale * _translation * glm::inverse(_scale) * glm::vec4(vertex.position, 1));
	vertex.position = glm::vec3(_scale * _translation * _rotation * glm::inverse(_translation) * glm::inverse(_scale) * glm::vec4(vertex.position, 1));
	vertex.normal = glm::vec3(_rotation * glm::vec4(0, 1, 0, 1));
	vertex.texcoord = { position.x, position.z };
	_mesh.vertices.push_back(vertex);
}

void Tile::generate_vertex_skirt(glm::vec3 position, glm::vec3 normal) {
	Vertex vertex;
	position.y = position.y - 0.05f; // Ugly
	vertex.position = glm::vec3(_scale * glm::vec4(position, 1));
	vertex.position = glm::vec3(_scale * _translation * glm::inverse(_scale) * glm::vec4(vertex.position, 1));
	vertex.position = glm::vec3(_scale * _translation * _rotation * glm::inverse(_translation) * glm::inverse(_scale) * glm::vec4(vertex.position, 1));
	vertex.normal = glm::vec3(_rotation * glm::vec4(0, 1, 0, 1));
	vertex.texcoord = { position.x, position.z };
	_mesh.vertices.push_back(vertex);
}

void Tile::generate_mesh() {
	float step = 1.0f / _resolution;
	float offset = 0.5f;
	unsigned int i = 0, j = 0;
	float x, z;

	for (i = 0; i <= _resolution; ++i) {
		if (i == 0) {
			for (j = 0; j <= _resolution; ++j) {
				x = i * step;
				z = j * step;
				glm::vec3 position(x - offset, 0, z - offset);
				if (j == 0) {
					generate_vertex_skirt(position, glm::cross(glm::vec3(x, 0, z), glm::vec3(0, 0, z)));
				}
				generate_vertex_skirt(position, glm::cross(glm::vec3(x, 0, z), glm::vec3(x, 0, 0)));
				if (j == _resolution) {
					generate_vertex_skirt(position, glm::cross(glm::vec3(x, 0, z), glm::vec3(0, 0, -z)));
				}
			}
		}
		for (j = 0; j <= _resolution; ++j) {
			x = i * step;
			z = j * step;
			glm::vec3 position(x - offset, 0, z - offset);
			if (j == 0) {
				generate_vertex_skirt(position, glm::cross(glm::vec3(x, 0, z), glm::vec3(0, 0, z)));
			}
			generate_vertex(position);
			if (j == _resolution) {
				generate_vertex_skirt(position, glm::cross(glm::vec3(x, 0, z), glm::vec3(0, 0, -z)));
			}
		}
		if (i == _resolution) {
			for (j = 0; j <= _resolution; ++j) {
				x = i * step;
				z = j * step;
				glm::vec3 position(x - offset, 0, z - offset);
				if (j == 0) {
					generate_vertex_skirt(position, glm::cross(glm::vec3(x, 0, z), glm::vec3(0, 0, z)));
				}
				generate_vertex_skirt(position, glm::cross(glm::vec3(x, 0, z), glm::vec3(x, 0, 0)));
				if (j == _resolution) {
					generate_vertex_skirt(position, glm::cross(glm::vec3(x, 0, z), glm::vec3(0, 0, -z)));
				}
			}
		}
	}

	unsigned int stride = _resolution + skirt_padding() + 1;
	for (i = 0; i < _resolution + skirt_padding(); ++i) {
		for (j = 0; j < _resolution + skirt_padding(); ++j) {
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
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	// I think _translation, _rotation, _scale is done in mesh generation.
	// Could probably be uploaded here instead for better performance ?
	glm::mat4 model;
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
}

void Tile::translateTile(const glm::vec3 &t) {
	glm::mat4 model = glm::translate(t);
}

unsigned int Tile::skirt_padding() {
	return 2;
}

void Tile::draw(const Lighting &lighting, const Camera & camera, double delta_time) {
	setup_draw(lighting, camera, delta_time);
	_mesh.draw(_shader, delta_time);
}

void Tile::draw_wireframe(const Lighting &lighting, const Camera & camera, double delta_time) {
	setup_draw(lighting, camera, delta_time);
	_mesh.draw_wireframe(_shader, delta_time);
}
