#include "QuadTree.h"
#include <iostream>
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtx\transform.hpp>

void QuadTree::draw(const Lighting &lighting, const Camera &camera, double delta_time) {
	if (_level < 2) {
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
	//std::cout << "Creating tile :  " << _extents << " " << _center.x << " " << _center.y << " " << _center.z << std::endl;
	//glm::mat4 trans = _translation;
	//trans[3] *= 0.25f;
	_patch = std::make_shared<Tile>(64, glm::mat4(_extents), _translation, _rotation, false);
}

void QuadTree::subdivide() {

	float scale = _extents * 0.5f;
	float offset = 0.5f;
	glm::vec3 trans = glm::vec3(_translation[3])*2.0f;
	glm::mat4 tnw = glm::translate(glm::vec3(trans.x - offset, trans.y, trans.z + offset));
	glm::mat4 tne = glm::translate(glm::vec3(trans.x + offset, trans.y, trans.z + offset));
	glm::mat4 tsw = glm::translate(glm::vec3(trans.x - offset, trans.y, trans.z - offset));
	glm::mat4 tse = glm::translate(glm::vec3(trans.x + offset, trans.y, trans.z - offset));
	
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
