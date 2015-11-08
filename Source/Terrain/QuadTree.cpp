#include "QuadTree.h"
#include <iostream>

void QuadTree::draw(const Lighting &lighting, const Camera &camera, double delta_time) {
	if (_level < 1) {
		if (_has_children) {
			_northwest->draw(lighting, camera, delta_time);
			_northeast->draw(lighting, camera, delta_time);
			_southwest->draw(lighting, camera, delta_time);
			_southeast->draw(lighting, camera, delta_time);
		}
		else {
			subdivide();
		}
		//std::cout << _level << std::endl;
	}
	else {

		//std::cout << _level << std::endl;

		if (!_has_patch) {
			create_patch();
			_has_patch = true;
		}
		else {
			_patch->draw_wireframe(lighting, camera, delta_time);
		}
	}
}

void QuadTree::draw_wireframe(const Lighting &lighting, const Camera &camera, double delta_time) {
	draw(lighting, camera, delta_time);
}

void QuadTree::create_patch() {
	std::cout << "Creating tile :  " << _extents << " " << _center.x << " " << _center.y << " " << _center.z << std::endl;
	_patch = std::make_shared<Tile>(16, glm::mat4(_extents * 0.5f), glm::translate(glm::mat4(1), glm::vec3(0.5, 0, 0)), glm::mat4(1), false);
}

void QuadTree::subdivide() {

	float offset = _extents * 0.5f;
	//Create 4 sub-quadtrees
	_northwest = std::make_shared<QuadTree>(glm::vec3(_center.x - offset, _center.y, _center.z + offset), offset);
	_northwest->_level = _level + 1;
	_northwest->_parent = this;

	_northeast = std::make_shared<QuadTree>(glm::vec3(_center.x + offset, _center.y, _center.z + offset), offset);
	_northeast->_level = _level + 1;
	_northeast->_parent = this;

	_southwest = std::make_shared<QuadTree>(glm::vec3(_center.x - offset, _center.y, _center.z - offset), offset);
	_southwest->_level = _level + 1;
	_southwest->_parent = this;

	_southeast = std::make_shared<QuadTree>(glm::vec3(_center.x + offset, _center.y, _center.z - offset), offset);
	_southeast->_level = _level + 1;
	_southeast->_parent = this;

	_has_children = true;
}
