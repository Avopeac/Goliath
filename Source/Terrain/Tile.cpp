#include "Tile.h"
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/noise.hpp>
#include "View/Renderer.h"

Tile::Tile(unsigned int resolution, const glm::mat4 &scale, const glm::mat4 &translation, const glm::mat4 &rotation)
	: Drawable(), _resolution(resolution), _translation(translation), _scale(scale), _rotation(rotation) {
	set_shader(Renderer::instance().get_standard_shader(), true);
	_material.albedo = glm::vec3(0.5, 0.31, 0.01);
	_material.absorption = 1.0f;
	_material.gaussian = 0.5f;
	_material.refraction = 1.0f;
	_premult_transf = _translation * _rotation * _scale;
	//Do this last!
	generate_mesh();
}

void Tile::generate_vertex(glm::vec3 position) {
	Vertex vertex;
	vertex.position = glm::vec3(_premult_transf * glm::vec4(position, 1.0));
	vertex.texcoord = { position.x, position.z };
	//Spherify
	vertex.position = glm::vec3(glm::normalize(vertex.position)) * 4.0f;
	//Do some offset
	vertex.position += glm::vec3(glm::normalize(vertex.position)) * height_function(2.0f, 0.65f, 5.0f, 0.01f, 0.1f, vertex.position);
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

	generate_normals();
	//Upload mesh
	_mesh.setup_mesh();
}

float Tile::height_function(float lacunarity, float dimension, float octaves, float scale, float offset, glm::vec3 &seed) {
	//Fractal height
	float value = 1.0f;
	unsigned int i, octs = (unsigned int)octaves;
	static std::vector<float> exponentials(octs + 1);
	static bool first = true;
	glm::vec2 p(seed);
	
	if (first) {
		for (i = 0; i <= octs; ++i) {
			exponentials[i] = pow(lacunarity, -dimension * (float)i);
		}
		first = false;
	}
	for (i = 1; i < octs; ++i) {
		value += ((glm::simplex(p) + offset) / offset) * exponentials[i];
		p *= 2.0f;
	}
	return value * scale;
}

void Tile::generate_normals() {
	for (unsigned int i = 0; i < _mesh.indices.size(); i += 3) {
		const int ia = _mesh.indices[i + 0];
		const int ib = _mesh.indices[i + 1];
		const int ic = _mesh.indices[i + 2];
		Vertex &v1 = _mesh.vertices[ia];
		Vertex &v2 = _mesh.vertices[ib];
		Vertex &v3 = _mesh.vertices[ic];
		glm::vec3 normal = glm::cross(v1.position - v2.position,
			v3.position - v2.position);
		//Account for OpenGL
		normal.y = -normal.y;
		_mesh.vertices[ia].normal += normal;
		_mesh.vertices[ib].normal += normal;
		_mesh.vertices[ic].normal += normal;
	}

	for (auto &v : _mesh.vertices) {
		v.normal = glm::abs(glm::normalize(v.normal));
		
	}
}

void Tile::setup_draw(const Lighting &lighting, const Camera & camera, double delta_time) {
	_shader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glUniform3fv(glGetUniformLocation(_shader->program, "albedo"), 1, glm::value_ptr(_material.albedo));
	glUniform1f(glGetUniformLocation(_shader->program, "roughness"), _material.roughness);
	glUniform1f(glGetUniformLocation(_shader->program, "gaussian"), _material.gaussian);
	glUniform1f(glGetUniformLocation(_shader->program, "absorption"), _material.absorption);
	glUniform1f(glGetUniformLocation(_shader->program, "refraction"), _material.refraction);
	glm::mat4 model;
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "model"), 1, GL_FALSE, glm::value_ptr(model));
}

void Tile::draw(const Lighting &lighting, const Camera & camera, double delta_time) {
	setup_draw(lighting, camera, delta_time);
	_mesh.draw(_shader, delta_time);
}

void Tile::draw_wireframe(const Lighting &lighting, const Camera & camera, double delta_time) {
	setup_draw(lighting, camera, delta_time);
	_mesh.draw_wireframe(_shader, delta_time);
}
