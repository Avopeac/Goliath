#include "Water.h"

#include <Thread/Message.h>
#include <Thread/MessageSystem.h>
#include <iostream>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtx/transform.hpp>
#include <Input/Input.h>
#include "DayNight.h"
#include "Planet.h"

class Water::WaterMessage : public Message {
public:
	WaterMessage(Water *ref) : _ref(ref) {}
	void virtual process() override { _ref->_setup(); }
private:
	Water *_ref;
};

// For some reason keeping this in class doesn't work...
static double WATER_BASE_LOD_LEVEL = 2.75;
static double WATER_MAX_LOD_LEVEL = 12;
static double WATER_BASE_TESS_LEVEL = 80;
static double WATER_MAX_TESS_LEVEL = 32;
static double WATER_WAVE_HEIGHT = 0.00005;
static double WATER_WAVE_FREQ = 800;
static int32_t WATER_OCTETS = 3;

static inline double compute_level_metric(const Camera & camera, double distance, double extents) {
	const double lod_factor = WATER_BASE_LOD_LEVEL;
	return distance - lod_factor * extents;
};

static inline double distance_to_patch(const Camera &camera, const glm::dvec3 &mid_point) {
	return glm::distance(mid_point, camera.get_deye());
};

Water::~Water() {
	glDeleteVertexArrays(1, &_VAO);
	glDeleteBuffers(1, &_VBO);
	glDeleteBuffers(1, &_EBO);
}

Water::Water(double radius, const glm::dmat4& translation, const glm::dmat4& rotation, const glm::dmat4& scale)
	: _water_level(radius), _lod_level(0), _translation(translation), _rotation(rotation), _scale(scale) {
	_init();
}

Water::Water(float radius, const glm::dmat4& translation, const glm::dmat4& rotation, const glm::dmat4& scale, double lod_level)
	: _water_level(radius), _lod_level(lod_level), _translation(translation), _rotation(rotation), _scale(scale) {
	_init();
}

void Water::draw(const Camera& camera, double delta_time) {
	_draw(camera, delta_time, false);
}

void Water::draw_wireframe(const Camera& camera, double delta_time) {
	_draw(camera, delta_time, true);
}

void Water::_init() {
	static bool gui_init_done = false;
	if (!gui_init_done) {
		TwAddSeparator(Input::_tw_bar, "Water", nullptr);
		TwAddVarRW(Input::_tw_bar, "LOD level", TW_TYPE_DOUBLE, &WATER_BASE_LOD_LEVEL, "min=0.0 max=1024.0 step=0.05");
		TwAddVarRW(Input::_tw_bar, "Max LOD level", TW_TYPE_DOUBLE, &WATER_MAX_LOD_LEVEL, "min=0.0 max=1024.0 step=1.0");
		TwAddVarRW(Input::_tw_bar, "Tess level", TW_TYPE_DOUBLE, &WATER_BASE_TESS_LEVEL, "min=0.0 max=1024.0 step=1.0");
		TwAddVarRW(Input::_tw_bar, "Max tess level", TW_TYPE_DOUBLE, &WATER_MAX_TESS_LEVEL, "min=0.0 max=1024.0 step=1.0");
		TwAddVarRW(Input::_tw_bar, "Wave height", TW_TYPE_DOUBLE, &WATER_WAVE_HEIGHT, "min=0.0 max=1.0 step=0.000001");
		TwAddVarRW(Input::_tw_bar, "Wave freq", TW_TYPE_DOUBLE, &WATER_WAVE_FREQ, "min=0.0 max=1000000.0 step=1.0");
		TwAddVarRW(Input::_tw_bar, "# Octets", TW_TYPE_INT32, &WATER_OCTETS, "min=1 max=20 step=1");
		gui_init_done = true;
	}

	_setup_done = _gl_setup_done = false;

	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	glGenBuffers(1, &_EBO);

	set_shader(ShaderStore::instance().get_shader_from_store(A_LITTLE_COOLER_WATER_SHADER_PATH));
	glm::dvec3 vec = _vertices[_vertices.size() / 2];

	MessageSystem::instance().post_noreturn(std::make_shared<WaterMessage>(this));
}

void Water::_setup() {
	unsigned int resolution = WATER_BASE_RESOLUTION;
	double step = 1.0 / resolution;
	double offset = 0.5;
	glm::dmat4 transform = _translation * _rotation * _scale;
	glm::dvec3 max(-std::numeric_limits<double>::max()), min(std::numeric_limits<double>::max());
	for (int x = 0; x <= resolution; ++x) {
		for (int z = 0; z <= resolution; ++z) {
			double cx = x * step - offset;
			double cz = z * step - offset;
			// Apply transform to base position
			glm::dvec3 vertex(transform *  glm::vec4(cx, 0, cz, 1.0));
			// Add normal while we're at it
			vertex = glm::normalize(vertex);
			_normals.push_back(vertex);
			// Set length to water level
			vertex = _water_level * vertex;
			_vertices.push_back(vertex);

			//Find max points
			if (max.x < vertex.x) { max.x = vertex.x; }
			if (max.y < vertex.y) { max.y = vertex.y; }
			if (max.z < vertex.z) { max.z = vertex.z; }
			//Find min points
			if (min.x > vertex.x) { min.x = vertex.x; }
			if (min.y > vertex.y) { min.y = vertex.y; }
			if (min.z > vertex.z) { min.z = vertex.z; }
		}
	}

	_center = glm::normalize((max + min) * 0.5) * _water_level;
	_extents = (max - min) * 0.5;

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

void Water::_update_lod(double rho) {
	if (_children.empty() && rho < 0.0 && _lod_level < WATER_MAX_LOD_LEVEL) {
		glm::dmat4 translation, rotation, scale;
		rotation = _rotation;
		double lod_level = _lod_level + 1;
		// XXX: Base scale seems to correspond entirely to water level as it is now
		double dscale = _water_level * glm::pow(0.5, lod_level);
		double offset = _water_level * glm::pow(0.5, lod_level+1);
		glm::dvec3 origin = glm::dvec3(_translation[3]);
		scale = glm::scale(glm::dvec3(dscale));

		glm::dvec3 nw_rotated_offset = glm::dvec3(_rotation * glm::dvec4(glm::dvec3(offset, 0, -offset), 1.0));
		translation = glm::translate(origin + nw_rotated_offset);
		_children.emplace_back(new Water(_water_level, translation, rotation, scale, lod_level));

		glm::dvec3 ne_rotated_offset = glm::dvec3(_rotation * glm::dvec4(glm::dvec3(-offset, 0, -offset), 1.0));
		translation = glm::translate(origin + ne_rotated_offset);
		_children.emplace_back(new Water(_water_level, translation, rotation, scale, lod_level));

		glm::dvec3 sw_rotated_offset = glm::dvec3(_rotation * glm::dvec4(glm::dvec3(offset, 0, offset), 1.0));
		translation = glm::translate(origin + sw_rotated_offset);
		_children.emplace_back(new Water(_water_level, translation, rotation, scale, lod_level));

		glm::dvec3 se_rotated_offset = glm::dvec3(_rotation * glm::dvec4(glm::dvec3(-offset, 0, offset), 1.0));
		translation = glm::translate(origin + se_rotated_offset);
		_children.emplace_back(new Water(_water_level, translation, rotation, scale, lod_level));
	}
	else if (!_children.empty() && rho > 0.0) {
		_children.clear();
	}
}

bool Water::_children_setup_done() {
	bool ret = !_children.empty();
	for (auto& child : _children) {
		ret = ret && child->_setup_done;
	}
	return ret;
}

void Water::_gl_setup() {
	std::vector<GLfloat> buffer(3 * (_vertices.size() + _normals.size()));
	buffer.resize(3 * (_vertices.size() + _normals.size()));

	// XXX: Probably unnecessary copying of mesh but I don't like OpenGL
	for (int i = 0; i < _vertices.size(); ++i) {
		buffer[i * 6] = _vertices[i].x;
		buffer[i * 6 + 1] = _vertices[i].y;
		buffer[i * 6 + 2] = _vertices[i].z;
		buffer[i * 6 + 3] = _normals[i].x;
		buffer[i * 6 + 4] = _normals[i].y;
		buffer[i * 6 + 5] = _normals[i].z;
	}

	glBindVertexArray(_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(GLfloat), buffer.data(), GL_STATIC_DRAW);

	// Vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	// Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));
	// Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);
}

void Water::_draw(const Camera& camera, double delta_time, bool wireframe) {
	if (!_setup_done) {
		return;
	}

	double extents = _water_level * glm::pow(0.5, _lod_level);
	double rho = compute_level_metric(camera, distance_to_patch(camera, _center), extents);
	_update_lod(rho);

	if (Planet::horizon_culling(camera.get_deye(), glm::dvec3(0, 0, 0), _water_level, _center, extents)) {
		return;
	}

	if (!camera.intersects_box(_center, _extents * 1.1)) {
		return;
	}

	if (_children_setup_done()) {
		// Draw children
		for (auto& child : _children) {
			child->_draw(camera, delta_time, wireframe);
		}
	}
	else {
		// Draw ourselves
		glBindVertexArray(_VAO);

		if (!_gl_setup_done) {
			_gl_setup();
			_gl_setup_done = true;
		}

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);

		_shader->use();
		auto time_now = std::chrono::steady_clock::now();
		std::chrono::duration<float> time_now_float = std::chrono::duration_cast<std::chrono::duration<float>>(time_now.time_since_epoch());
		glUniform1f(glGetUniformLocation(_shader->program, "globTime"), time_now_float.count());
		glUniform3fv(glGetUniformLocation(_shader->program, "sunlightDir"), 1, glm::value_ptr(glm::vec3(DayNight::instance().get_sun())));
		glUniform3fv(glGetUniformLocation(_shader->program, "wCameraPos"), 1, glm::value_ptr(glm::vec3(camera.get_deye())));
		glUniform1f(glGetUniformLocation(_shader->program, "near"), camera.get_near());
		glUniform1f(glGetUniformLocation(_shader->program, "far"), camera.get_far());
		glUniform1f(glGetUniformLocation(_shader->program, "quadtree_level"), _lod_level);
		glUniform1f(glGetUniformLocation(_shader->program, "baseTessellationLevel"), WATER_BASE_TESS_LEVEL);
		glUniform1f(glGetUniformLocation(_shader->program, "waveFreq"), WATER_WAVE_FREQ);
		glUniform1i(glGetUniformLocation(_shader->program, "maxLODLevel"), WATER_MAX_LOD_LEVEL);
		// Uploaded vertices in world space already
		glm::mat4 mvp_gpu(camera.get_dprojection() * camera.get_dview());
		glUniformMatrix4fv(glGetUniformLocation(_shader->program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp_gpu));

		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		// Tell OpenGL that every patch has 3 vertices
		glPatchParameteri(GL_PATCH_VERTICES, 3);

		// First draw filler layer (fixes gaps)
		glUniform1f(glGetUniformLocation(_shader->program, "waveHeight"), 0.0);
		glUniform1i(glGetUniformLocation(_shader->program, "maxOctets"), 0);
		glUniform1f(glGetUniformLocation(_shader->program, "maxTessLevel"), 1.0);
		glDrawElements(GL_PATCHES, _indices.size(), GL_UNSIGNED_INT, nullptr);

		// Then draw for real
		glUniform1f(glGetUniformLocation(_shader->program, "waveHeight"), WATER_WAVE_HEIGHT);
		glUniform1i(glGetUniformLocation(_shader->program, "maxOctets"), WATER_OCTETS);
		glUniform1f(glGetUniformLocation(_shader->program, "maxTessLevel"), WATER_MAX_TESS_LEVEL);
		glDrawElements(GL_PATCHES, _indices.size(), GL_UNSIGNED_INT, nullptr);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(0);
	}
}
