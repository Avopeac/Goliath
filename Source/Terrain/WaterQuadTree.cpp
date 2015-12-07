#include "WaterQuadTree.h"
#include <iostream>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtx/transform.hpp>
#include "Thread/MessageSystem.h"

WaterQuadTree::WaterQuadTree(const glm::dmat4& rotation, const glm::dmat4& translation, double extents, double radii, std::shared_ptr<Shader> shader)
	: Drawable(), _translation(translation), _rotation(rotation), _extents(extents), _radii(radii) {
	set_shader(shader);
	create_patch();
}

void WaterQuadTree::draw(const Camera &camera, double delta_time) {
	if (!setup_done()) { return; }
	if (!camera.intersects_box(_patch->get_center(), _patch->get_extents())) {
		remove_children();
		return;
	}

	// Get LOD metric to see if we should draw child quads or just draw this one
	double rho = compute_level_metric(camera, distance_to_patch(camera, _patch->get_center()));
	if (rho >= 0.0 || _level > _deepest_level) {
		_patch->draw(camera, delta_time);
		remove_children();
	}
	else {
		// If we already have created the children then just draw them
		if (_has_children) {
			if (_northwest->setup_done() && _northeast->setup_done() &&
				_southwest->setup_done() && _southeast->setup_done()) {
				_northwest->draw(camera, delta_time);
				_northeast->draw(camera, delta_time);
				_southwest->draw(camera, delta_time);
				_southeast->draw(camera, delta_time);

			}
			else {
				// Draw this tile until all children is setup
				_patch->draw(camera, delta_time);
			}
		}
		else { // Else create children and draw this tile
			subdivide();
			_patch->draw(camera, delta_time);
		}
	}
}

void WaterQuadTree::draw_wireframe(const Camera &camera, double delta_time) {
	//Just draw as usual
	draw(camera, delta_time);
}

void WaterQuadTree::create_patch() {
	_patch = std::make_shared<WaterTile>(_translation, glm::scale(glm::dvec3(_extents)), _rotation, _radii, _shader);
}

void WaterQuadTree::subdivide() {
	double scale(_extents * 0.5);
	double offset(_extents * 0.25);
	glm::dvec3 origin(_translation[3]);
	glm::dvec3 nw_roff(_rotation * glm::dvec4(glm::dvec3(offset, 0, -offset), 1.0));
	glm::dvec3 ne_roff(_rotation * glm::dvec4(glm::dvec3(-offset, 0, -offset), 1.0));
	glm::dvec3 sw_roff(_rotation * glm::dvec4(glm::dvec3(offset, 0, offset), 1.0));
	glm::dvec3 se_roff(_rotation * glm::dvec4(glm::dvec3(-offset, 0, offset), 1.0));
	//Create 4 sub-quadtrees, pass on shaders
	_northwest = std::make_shared<WaterQuadTree>(_rotation, glm::translate(origin + nw_roff), scale, _radii, _shader); _northwest->_level = _level + 1; _northwest->_parent = this;
	_northeast = std::make_shared<WaterQuadTree>(_rotation, glm::translate(origin + ne_roff), scale, _radii, _shader); _northeast->_level = _level + 1; _northeast->_parent = this;
	_southwest = std::make_shared<WaterQuadTree>(_rotation, glm::translate(origin + sw_roff), scale, _radii, _shader); _southwest->_level = _level + 1; _southwest->_parent = this;
	_southeast = std::make_shared<WaterQuadTree>(_rotation, glm::translate(origin + se_roff), scale, _radii, _shader); _southeast->_level = _level + 1; _southeast->_parent = this;
	//This quadtree now has children
	_has_children = true;
}

// Recursively removes all children if no children is currently being created
bool WaterQuadTree::remove_children() {
	bool removal = false;
	if (_has_children && _northwest->setup_done() && _northeast->setup_done() && _southwest->setup_done() && _southeast->setup_done()) {
		if (_northwest->remove_children() && _northeast->remove_children() &&
			_southwest->remove_children() && _southeast->remove_children()) {
			_northwest->_patch.reset(); _northwest.reset();
			_northeast->_patch.reset(); _northeast.reset();
			_southwest->_patch.reset(); _southwest.reset();
			_southeast->_patch.reset(); _southeast.reset();
			_has_children = false;
			removal = true;
		}
	}
	else if (!_has_children) {
		removal = true;
	}
	return removal;
}