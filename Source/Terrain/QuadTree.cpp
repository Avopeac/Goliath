#include "QuadTree.h"
#include "Thread/MessageSystem.h"
#include <iostream>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtx/transform.hpp>

QuadTree::QuadTree(): Drawable() {
	set_shader(ShaderStore::instance().get_shader_from_store(GROUND_SHADER_PATH));
	create_patch();
	_has_patch = true; // TODO: Remove ?
}

QuadTree::QuadTree(const glm::mat4& rotation, const glm::mat4& translation, float extents)
	: Drawable(), _translation(translation), _rotation(rotation), _extents(extents) {
	set_shader(ShaderStore::instance().get_shader_from_store(GROUND_SHADER_PATH));
	create_patch();
	_has_patch = true;
}

QuadTree::QuadTree(const glm::mat4& rotation, const glm::mat4& translation, float extents, std::shared_ptr<Shader> shader)
	: Drawable(), _translation(translation), _rotation(rotation), _extents(extents) {
	set_shader(shader);
	create_patch();
	_has_patch = true;
}

void QuadTree::draw(const Camera &camera, double delta_time) {

	if (!setup_done()) {
		return;
	}

	// Check if we have to draw this patch
	// TODO Calculate this without generated mesh if possible ?
	glm::vec3 mid_point = _patch->_mesh.vertices[_patch->_mesh.vertices.size() / 2].position;
	glm::vec3 scale = glm::vec3(_extents, _extents, _extents);
	if (!camera.intersects_box(mid_point, scale)) {
		return;
	}

	// Get LOD metric to see if we should draw child quads or just draw this one
	double rho = compute_level_metric(camera, distance_to_patch(camera, mid_point));
	if (rho >= _TAU || _level > _DEEPEST_LEVEL) {
		// morph stuff

		if (!dont_morph) {
				_morph_value += delta_time / 0.5f;
			_morph_value = glm::min(1.0f, _morph_value);
			_patch->morph_vertices(_morph_value);
		}
		if (_morph_value == 1.0f) {
			dont_morph = true;
		}


		_patch->draw(camera, delta_time);
	}
	else {
		dont_morph = false;

		// If we already have created the childs then just draw them
		if (_has_children) {
			if (_northwest->setup_done() && _northeast->setup_done() &&
				_southwest->setup_done() && _southeast->setup_done()) {
				_northwest->draw(camera, delta_time);
				_northeast->draw(camera, delta_time);
				_southwest->draw(camera, delta_time);
				_southeast->draw(camera, delta_time);
			}
			else {
				// Draw ourselves until all children is setup
				_patch->draw(camera, delta_time);
			}
		}
		// else create children and draw ourselves
		else {
			subdivide();
			_patch->draw(camera, delta_time);
		}
	}
}

double QuadTree::distance_nearest_corner(const Camera &camera) {
	return glm::abs(glm::dot((glm::vec3(_translation[3]) - camera.get_eye()), -glm::vec3(camera.get_view()[2])) - _extents);
}

double QuadTree::distance_to_patch(const Camera &camera, glm::vec3 mid_point) {
	//glm::vec3 midPoint = _patch->_mesh.vertices[_patch->_mesh.vertices.size() / 2].position;
	return glm::max(0.0f, glm::distance(mid_point, camera.get_eye()) - _extents);
}

void QuadTree::draw_wireframe(const Camera &camera, double delta_time) {
	//Just draw as usual
	draw(camera, delta_time);
}

void QuadTree::create_patch() {
	_patch = std::make_shared<PlanetTile>(_translation, glm::scale(glm::vec3(_extents)), _rotation, _shader);
}

double QuadTree::compute_level_metric(const Camera &camera, double distance) {
	//double omega = 2.0 * distance * glm::tan(glm::radians(camera.get_horizontal_fov()) * 0.5);
	//double epsilon = 0.001f;
	//double rho = epsilon * Application::width / omega;
	float K = 0.25f; // 0.25f is pretty good
	float lol_metric = glm::pow((_level + 1.0f), K*(_level + 1.0f)) - 1.0f/(float)distance; // K*(_level + 1.0f)*(_level + 1.0f)
	return lol_metric;
}

void QuadTree::subdivide() {
	float scale = _extents * 0.5f;
	float offset = _extents * 0.25f;
	glm::vec3 nw_rotated_offset = glm::vec3(_rotation * glm::vec4(glm::vec3(offset, 0, -offset), 1.0f));
	glm::vec3 ne_rotated_offset = glm::vec3(_rotation * glm::vec4(glm::vec3(-offset, 0, -offset), 1.0f));
	glm::vec3 sw_rotated_offset = glm::vec3(_rotation * glm::vec4(glm::vec3(offset, 0, offset), 1.0f));
	glm::vec3 se_rotated_offset = glm::vec3(_rotation * glm::vec4(glm::vec3(-offset, 0, offset), 1.0f));
	glm::vec3 origin = glm::vec3(_translation[3]);
	//Create 4 sub-quadtrees, pass on shaders
	_northwest = std::make_shared<QuadTree>(_rotation, glm::translate(origin + nw_rotated_offset), scale, _shader);
	_northwest->_level = _level + 1;
	_northwest->_parent = this;
	_northeast = std::make_shared<QuadTree>(_rotation, glm::translate(origin + ne_rotated_offset), scale, _shader);
	_northeast->_level = _level + 1;
	_northeast->_parent = this;
	_southwest = std::make_shared<QuadTree>(_rotation, glm::translate(origin + sw_rotated_offset), scale, _shader);
	_southwest->_level = _level + 1;
	_southwest->_parent = this;
	_southeast = std::make_shared<QuadTree>(_rotation, glm::translate(origin + se_rotated_offset), scale, _shader);
	_southeast->_level = _level + 1;
	_southeast->_parent = this;
	//This quadtree now has children
	_has_children = true;
}
