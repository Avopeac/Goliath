#include "QuadTree.h"
#include <iostream>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtx/transform.hpp>
#include "Thread/MessageSystem.h"
#include "Terrain/Planet.h"

QuadTree::QuadTree(const glm::dmat4& rotation, const glm::dmat4& translation, double extents, double radii, std::shared_ptr<Shader> shader)
	: Drawable(), _translation(translation), _rotation(rotation), _extents(extents), _radii(radii) {
	set_shader(shader);
	create_patch();
}

void QuadTree::draw(const Camera &camera, double delta_time) {

	if (!setup_done()) { return; }

	_lifetime += delta_time;

	Vertex v = _patch->mesh.vertices[_patch->mesh.vertices.size() / 2];
	glm::vec3 cen = _patch->get_center();

	if (_lifetime > _max_lifetime && Planet::horizon_culling(camera.get_deye(), glm::dvec3(0, 0, 0), _radii, v.position + cen, _extents)) {
		remove_children();
		return;
	}

	if (_lifetime > _max_lifetime && !camera.intersects_box(_patch->get_center(), _patch->get_extents())) {
		remove_children();
		return;
	}

	// Get LOD metric to see if we should draw child quads or just draw this one
	double rho = compute_level_metric(camera, distance_to_patch(camera, v.position + cen));//_patch->get_center()
	if (rho >= 0.0 || _level > _deepest_level) {
		_lifetime = 0.0;
		_patch->draw(camera, delta_time);

		if (_lifetime > _max_lifetime)
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

void QuadTree::draw_wireframe(const Camera &camera, double delta_time) {
	draw(camera, delta_time);
}

void QuadTree::create_patch() {
	_patch = std::make_shared<PlanetTile>(_translation, glm::scale(glm::dvec3(_extents)), _rotation, _radii, _shader);
}

void QuadTree::subdivide() {
	double scale(_extents * 0.5);
	double offset(_extents * 0.25);
	glm::dvec3 origin(_translation[3]);
	glm::dvec3 nw_roff(_rotation * glm::dvec4(glm::dvec3(offset, 0, -offset), 1.0));
	glm::dvec3 ne_roff(_rotation * glm::dvec4(glm::dvec3(-offset, 0, -offset), 1.0));
	glm::dvec3 sw_roff(_rotation * glm::dvec4(glm::dvec3(offset, 0, offset), 1.0));
	glm::dvec3 se_roff(_rotation * glm::dvec4(glm::dvec3(-offset, 0, offset), 1.0));
	//Create 4 sub-quadtrees, pass on shaders
	_northwest = std::make_shared<QuadTree>(_rotation, glm::translate(origin + nw_roff), scale, _radii, _shader); _northwest->_level = _level + 1; _northwest->_parent = this;
	_northeast = std::make_shared<QuadTree>(_rotation, glm::translate(origin + ne_roff), scale, _radii, _shader); _northeast->_level = _level + 1; _northeast->_parent = this;
	_southwest = std::make_shared<QuadTree>(_rotation, glm::translate(origin + sw_roff), scale, _radii, _shader); _southwest->_level = _level + 1; _southwest->_parent = this;
	_southeast = std::make_shared<QuadTree>(_rotation, glm::translate(origin + se_roff), scale, _radii, _shader); _southeast->_level = _level + 1; _southeast->_parent = this;
	//This quadtree now has children
	_has_children = true;
}

// Recursively removes all children if no children is currently being created
bool QuadTree::remove_children() {
	bool removal = false;
	if (_has_children && _northwest->setup_done() && _northeast->setup_done() && _southwest->setup_done() && _southeast->setup_done()) {
		if (_northwest->remove_children() && _northeast->remove_children() &&
			_southwest->remove_children() && _southeast->remove_children()) {
			_northwest->_patch.reset(); _northwest.reset();
			_northeast->_patch.reset(); _northeast.reset();
			_southwest->_patch.reset(); _southwest.reset();
			_southeast->_patch.reset(); _southeast.reset();
			_northwest = nullptr;
			_northeast = nullptr;
			_southwest = nullptr;
			_southeast = nullptr;
			_has_children = false;
			removal = true;
		}
	}
	else if (!_has_children) {
		removal = true;
	}
	return removal;
}