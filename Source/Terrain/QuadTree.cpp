#include "QuadTree.h"
#include <iostream>
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtx\transform.hpp>

void QuadTree::draw(const Lighting &lighting, const Camera &camera, double delta_time) {
	float rho = compute_level_metric(camera, distance_nearest_corner(camera));
	//if (rho <= _screen_space_error || _level > 2) {
	if (_level > 5) {
		if (_has_patch) {
			_patch->draw(lighting, camera, delta_time);
		}
		else {
			create_patch();
			_has_patch = true;
		}
	}
	else {
		if (_has_children) {
			_northwest->draw(lighting, camera, delta_time);
			_northeast->draw(lighting, camera, delta_time);
			_southwest->draw(lighting, camera, delta_time);
			_southeast->draw(lighting, camera, delta_time);
		}
		else {
			subdivide();
		}
	}
}

float QuadTree::distance_nearest_corner(const Camera &camera) {
	glm::vec3 view_vec(camera.get_view()[2]);
	return glm::abs(glm::dot(_origin - camera.get_eye(), view_vec) + _extents);
}

void QuadTree::draw_wireframe(const Lighting &lighting, const Camera &camera, double delta_time) {
	draw(lighting, camera, delta_time);
}

void QuadTree::create_patch() {
	_patch = std::make_shared<Tile>(4, glm::scale(glm::vec3(_extents)), _translation, _rotation);
}

float QuadTree::compute_level_metric(const Camera &camera, float distance) {
	//double omega = 2.0 * distance * glm::tan(glm::radians(camera.get_horizontal_fov()) * 0.5);
	//double epsilon = 0.001f;
	//double rho = epsilon * Application::width / omega;
	float lol_metric = distance * (_level + 1.0f);
	return lol_metric;
}

void QuadTree::subdivide() {
	float scale = _extents * 0.5f;
	float offset = _extents * 0.25f;
	glm::vec3 nw_rotated_offset = glm::vec3(_rotation * glm::vec4(glm::vec3(offset, 0, -offset), 1.0f));
	glm::vec3 ne_rotated_offset = glm::vec3(_rotation * glm::vec4(glm::vec3(-offset, 0, -offset), 1.0f));
	glm::vec3 sw_rotated_offset = glm::vec3(_rotation * glm::vec4(glm::vec3(offset, 0, offset), 1.0f));
	glm::vec3 se_rotated_offset = glm::vec3(_rotation * glm::vec4(glm::vec3(-offset, 0, offset), 1.0f));
	glm::vec3 translation = glm::vec3(_translation[3]);
	glm::mat4 tnw = glm::translate(translation + nw_rotated_offset);
	glm::mat4 tne = glm::translate(translation + ne_rotated_offset);
	glm::mat4 tsw = glm::translate(translation + sw_rotated_offset);
	glm::mat4 tse = glm::translate(translation + se_rotated_offset);
	//Create 4 sub-quadtrees
	_northwest = std::make_shared<QuadTree>(_rotation, tnw, scale);
	_northwest->_level = _level + 1;
	_northwest->_parent = this;
	_northeast = std::make_shared<QuadTree>(_rotation, tne, scale);
	_northeast->_level = _level + 1;
	_northeast->_parent = this;
	_southwest = std::make_shared<QuadTree>(_rotation, tsw, scale);
	_southwest->_level = _level + 1;
	_southwest->_parent = this;
	_southeast = std::make_shared<QuadTree>(_rotation, tse, scale);
	_southeast->_level = _level + 1;
	_southeast->_parent = this;
	_has_children = true;
}
