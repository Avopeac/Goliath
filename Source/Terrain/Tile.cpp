#include "Tile.h"
#include <GLM/gtc/noise.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtx/compatibility.hpp>
#include "View/Renderer.h"

Tile::Tile() : Drawable() {

}

Tile::Tile(unsigned int resolution, const glm::mat4 &scale, const glm::mat4 &translation, const glm::mat4 &rotation)
	: Drawable(), _resolution(resolution), _translation(translation), _scale(scale), _rotation(rotation), _inverse_resolution(1.0f / _resolution) {
	_premult_transf = _translation * _rotation * _scale;
}

void Tile::setup(unsigned int resolution, const glm::mat4 &scale, const glm::mat4 &translation, const glm::mat4 &rotation) {
	_resolution = resolution;
	_translation = translation;
	_scale = scale;
	_rotation = rotation;
	_premult_transf = _translation * _rotation * _scale;
	_inverse_resolution = 1.0f / _resolution;
}

void Tile::generate_vertex(glm::vec3 position) {
	Vertex vertex;
	vertex.position = glm::vec3(_premult_transf * glm::vec4(position, 1.0));
	glm::vec3 v_pos_n = glm::normalize(vertex.position);
	float height = get_height(v_pos_n, 4.0f, 0.1f, 12.0f, 2.0f, 0.6f);
	vertex.color = get_color_ramp(4.0f, 5.0f, height);
	vertex.position = v_pos_n * height;
	vertex.normal = get_central_difference_normal(position);
	vertex.texcoord = { position.x, position.z };
	_mesh.vertices.push_back(vertex);
}

void Tile::generate_skirt_vertex(glm::vec3 position) {
	Vertex vertex;
	vertex.position = glm::vec3(_premult_transf * glm::vec4(position, 1));
	glm::vec3 v_pos_n = glm::normalize(vertex.position);
	vertex.position = 2.0f * _skirt_ratio * v_pos_n;
	vertex.normal = glm::vec3(0, 0, 0);
	vertex.texcoord = { position.x, position.z };
	_mesh.vertices.push_back(vertex);
}

float Tile::get_height(const glm::vec3 &position, float radius, float scale, float octaves, float lacunarity, float dimension) {
	float value = 1.0;
	static std::vector<float> exponentials((unsigned int)octaves + 1);
	static bool first = true;
	glm::vec3 frac_position(position);
	int i;
	if (first) {
		for (i = 0; i <= octaves; ++i) {
			exponentials[i] = pow(lacunarity, -dimension * i);
		}
		first = false;
	}
	for (i = 1; i < octaves; ++i) {
		value += (glm::simplex(frac_position) + offset) * exponentials[i];
		frac_position *= lacunarity;
	}
	return radius + glm::abs(value) * scale;
}

glm::vec3 Tile::get_color_ramp(float min, float max, float height) {
	float ratio = height / (max - min);
	glm::vec3 bedrock(0.2, 0.2, 0.2);
	glm::vec3 green(0.1, 0.5, 0.1);
	return green;
}

glm::vec3 Tile::get_central_difference_normal(const glm::vec3 &position) {
	glm::vec3 up, down, left, right;
	up = glm::normalize(glm::vec3(_premult_transf * glm::vec4(position.x, 0, position.z + _inverse_resolution, 1.0)));
	up *= get_height(up, 4.0f, 0.1f, 6.0f, 2.0f, 0.6f);
	down = glm::normalize(glm::vec3(_premult_transf * glm::vec4(position.x, 0, position.z - _inverse_resolution, 1.0)));
	down *= get_height(down, 4.0f, 0.1f, 6.0f, 2.0f, 0.6f);
	left = glm::normalize(glm::vec3(_premult_transf * glm::vec4(position.x - _inverse_resolution, 0, position.z, 1.0)));
	left *= get_height(left, 4.0f, 0.1f, 6.0f, 2.0f, 0.6f);
	right = glm::normalize(glm::vec3(_premult_transf * glm::vec4(position.x + _inverse_resolution, 0, position.z, 1.0)));
	right *= get_height(right, 4.0f, 0.1f, 6.0f, 2.0f, 0.6f);
	return glm::cross(up - down, right - left);
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
}

void Tile::upload_mesh() {
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
