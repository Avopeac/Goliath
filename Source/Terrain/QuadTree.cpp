#include "QuadTree.h"
#include <iostream>
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtx\transform.hpp>

void QuadTree::draw(const Camera &camera, double delta_time) {

	// Always create the patch if there is none so we can check bounds and calculate lod metric
	if (!_has_patch) {
		create_patch();
		_has_patch = true;
	}

	// Check if we have to draw this patch
	glm::vec3 mid_point = _patch->_mesh.vertices[_patch->_mesh.vertices.size() / 2].position;
	glm::vec3 scale = glm::vec3(_extents, _extents, _extents);
	if (!camera.intersects_box(mid_point, scale)) {
		return;
	}

	// Get LOD metric to see if we should draw child quads or just draw this one
	double rho = compute_level_metric(camera, distance_to_patch(camera, mid_point));
	if (rho >= _TAU || _level > _DEEPEST_LEVEL) {
		_patch->draw_wireframe(camera, delta_time);
	}
	else {
		// If we already have created the childs then just draw them
		if (_has_children) {
			_northwest->draw(camera, delta_time);
			_northeast->draw(camera, delta_time);
			_southwest->draw(camera, delta_time);
			_southeast->draw(camera, delta_time);
		}
		// else create childs
		else {
			subdivide();
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
	_patch = std::make_shared<Tile>(2, glm::scale(glm::vec3(_extents)), _translation, _rotation);
	_patch->generate_mesh();
	_patch->upload_mesh();
	_patch->set_shader(_shader); //Set to quadtree shader
}

double QuadTree::compute_level_metric(const Camera &camera, double distance) {
	//double omega = 2.0 * distance * glm::tan(glm::radians(camera.get_horizontal_fov()) * 0.5);
	//double epsilon = 0.001f;
	//double rho = epsilon * Application::width / omega;
	float K = 0.15f; // 0.15f is pretty good
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
	_northwest = std::make_shared<QuadTree>(_rotation, glm::translate(origin + nw_rotated_offset), scale);
	_northwest->_shader = _shader;
	_northwest->_level = _level + 1;
	_northwest->_parent = this;
	_northeast = std::make_shared<QuadTree>(_rotation, glm::translate(origin + ne_rotated_offset), scale);
	_northeast->_shader = _shader;
	_northeast->_level = _level + 1;
	_northeast->_parent = this;
	_southwest = std::make_shared<QuadTree>(_rotation, glm::translate(origin + sw_rotated_offset), scale);
	_southwest->_shader = _shader;
	_southwest->_level = _level + 1;
	_southwest->_parent = this;
	_southeast = std::make_shared<QuadTree>(_rotation, glm::translate(origin + se_rotated_offset), scale);
	_southeast->_shader = _shader;
	_southeast->_level = _level + 1;
	_southeast->_parent = this;
	//This quadtree now has children
	_has_children = true;
}
