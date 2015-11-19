#include "Water.h"
#include <Thread/Message.h>
#include <Thread/MessageSystem.h>
#include <iostream>
#include <GLM/gtc/type_ptr.hpp>

class Water::WaterMessage : public Message {
public:
	WaterMessage(Water *ref) : _ref(ref) {}
	void virtual process() override {
		_ref->_setup();
	}
private:
	Water *_ref;
};

Water::Water(const glm::mat4& translation, const glm::mat4& scale, const glm::mat4& rotation)
	: _translation(translation), _scale(scale), _rotation(rotation) {
	_init();
}

void Water::draw(const Camera& camera, double delta_time) {
	_draw(camera, delta_time, false);
}

void Water::draw_wireframe(const Camera& camera, double delta_time) {
	_draw(camera, delta_time, true);
}

void Water::_init() {
	_setup_done = _gl_setup_done = false;

	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	glGenBuffers(1, &_EBO);

	set_shader(ShaderStore::instance().get_shader_from_store(WATER_SHADER_PATH));

	MessageSystem::instance().post_noreturn(std::make_shared<WaterMessage>(this));
}

void Water::_setup() {
	unsigned int resolution = _base_resolution;
	float step = 1.0f / resolution;
	float offset = 0.5f;
	glm::mat4 transform = _translation * _rotation * _scale;

	for (int x = 0; x <= resolution; ++x) {
		for (int z = 0; z <= resolution; ++z) {
			float cx = x * step - offset;
			float cz = z * step - offset;

			// Apply transform to base position
			glm::vec3 vertex = glm::vec3(transform *  glm::vec4(cx, 0, cz, 1.0));
			
			// Add normal while we're at it
			vertex = glm::normalize(vertex);
			_normals.push_back(vertex);

			// Set length to water level
			vertex = _water_level * vertex;
			_vertices.push_back(vertex);
		}
	}

	// Set up indices
	int stride = resolution + 1;
	for (int x = 0; x < resolution; ++x) {
		for (int z = 0; z < resolution; ++z) {
			_indices.push_back(x + 1 + z * stride);
			_indices.push_back(x + (z + 1) * stride);
			_indices.push_back(x + z * stride);
			_indices.push_back(x + 1 + (z + 1) * stride);
			_indices.push_back(x + (z + 1) * stride);
			_indices.push_back(x + 1 + z * stride);
		}
	}

	_setup_done = true;
}

void Water::_gl_setup() {
	std::vector<GLfloat> buffer(3 * (_vertices.size() + _normals.size()));
	buffer.resize(3 * (_vertices.size() + _normals.size()));

	for (int i = 0; i < _vertices.size(); ++i) {
		buffer[i * 3] = _vertices[i].x;
		buffer[i * 3 + 1] = _vertices[i].y;
		buffer[i * 3 + 2] = _vertices[i].z;
	}
	for (int i = _vertices.size(); i < _vertices.size() + _normals.size(); ++i) {
		buffer[i * 3] = _normals[i].x;
		buffer[i * 3 + 1] = _normals[i].y;
		buffer[i * 3 + 2] = _normals[i].z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(GLfloat), buffer.data(), GL_STATIC_DRAW);

	// Vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	// Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat) * _vertices.size()));
	// Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);
}

void Water::_draw(const Camera& camera, double delta_time, bool wireframe) {
	if (!_setup_done) {
		return;
	}

	glBindVertexArray(_VAO);

	if (!_gl_setup_done) {
		_gl_setup();
		_gl_setup_done = true;
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	_shader->use();
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

	//Draw with index list
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, nullptr);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(0);
}