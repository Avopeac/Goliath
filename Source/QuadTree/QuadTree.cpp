#include <thread>
#include <limits>
#include "QuadTree.h"
#include "FractalHeightFunction.h"

int QuadTree::_id_counter = 0;

QuadTree::QuadTree(int level, int max_deviations, const AABB<double> &box) : _box(box), _level(level), _max_deviations(max_deviations) {
	_this_id = _id_counter++;
	generate();
	_tile->_tileMesh.SetupMesh();
};

QuadTree::QuadTree(const AABB<double> &box) : QuadTree(0, DEFAULT_DEVATIONS_ZERO, box) {};

void QuadTree::generate() {
	_tile = std::make_shared<Tile>();
	glm::dmat4 rotation(1);
	glm::dmat4 scale(2.0 * _box.GetExtent());
	_tile->generate(DEFAULT_TILE_SIZE, rotation, _box.GetCenter(), scale, false, true);
}

bool QuadTree::has_children() {
	return _northwest != nullptr && _northeast != nullptr && _southwest != nullptr && _southeast != nullptr;
}

bool QuadTree::has_leaf() {
	return _tile != nullptr;
}

bool QuadTree::child_data_resident() {
	return _northwest->has_leaf() && _northeast->has_leaf() && _southwest->has_leaf() && _southeast->has_leaf();
}

double QuadTree::distance_nearest_corner(const Camera &camera) {
	vec3 eye = camera.GetEyeVector();
	vec3 c = _box.GetCenter();
	double o = _box.GetExtent();

	std::vector<vec3> corners;
	corners.push_back(eye - vec3(c.x - o, c.y, c.z + o));
	corners.push_back(eye - vec3(c.x + o, c.y, c.z + o));
	corners.push_back(eye - vec3(c.x - o, c.y, c.z - o));
	corners.push_back(eye - vec3(c.x + o, c.y, c.z - o));
	corners.push_back(eye - c);

	double max = std::numeric_limits<double>().max();
	vec3 min(max, max, max);
	for (auto it = corners.begin(); it != corners.end(); ++it) {
		vec3 elem = *it;
		if (glm::dot(elem, elem), glm::dot(min, min)) {
			min = elem;
		}
	}

	return glm::dot(min,min);
}

void QuadTree::draw(Shader &shader, const Camera &camera) {
	double rho = compute_level_metric(camera, distance_nearest_corner(camera));

	std::cout << rho << std::endl;

	//bool children = has_children();
	//bool children_ready = children ? child_data_resident() : false;

	/*if (rho <= DEFAULT_SCREEN_SPACE_ERROR || (children && !children_ready)) {
		if (has_leaf()) {
			_tile->_tileMesh.DrawWireframe(shader);
		}
		else {
			generate();
			_tile->_tileMesh.SetupMesh();
		}
	}
	else {

		if (has_children()) {
			_northwest->draw(shader, camera);
			_northeast->draw(shader, camera);
			_southwest->draw(shader, camera);
			_southeast->draw(shader, camera);
		}
		else {
			std::cout << "Splitting at level " << _level << std::endl;
			_tile = nullptr;
			split();
		}
	}*/

	if (rho <= DEFAULT_SCREEN_SPACE_ERROR) {
		if (has_leaf()) {
			_tile->_tileMesh.DrawWireframe(shader);
		}
		else {
			generate();
			_tile->_tileMesh.SetupMesh();
		}
	}
	else {

		if (has_children()) {
			_northwest->draw(shader, camera);
			_northeast->draw(shader, camera);
			_southwest->draw(shader, camera);
			_southeast->draw(shader, camera);
		}
		else {
			//std::cout << "Splitting at level " << _level << std::endl;
			_tile = nullptr;
			split();
		}
	}
}

void QuadTree::merge() {
	_northwest = nullptr;
	_northeast = nullptr;
	_southwest = nullptr;
	_southeast = nullptr;
	generate();
}

double QuadTree::compute_level_metric(const Camera &camera, double distance) {
	double kappa = Application::GetWindowWidth() / (2.0 * glm::tan(glm::radians(camera.GetFovx() * 0.5)));
	double rho = (_max_deviations * _max_deviations * kappa * kappa) / distance;
	return rho;
}

void QuadTree::split() {
	vec3 center = _box.GetCenter();
	double offset = 0.5 * _box.GetExtent();
	_northwest = std::make_shared<QuadTree>(_level + 1, _max_deviations / 2, AABB<double>(vec3(center.x - offset, center.y, center.z + offset), offset));
	_northwest->parent = this;
	_northeast = std::make_shared<QuadTree>(_level + 1, _max_deviations / 2, AABB<double>(vec3(center.x + offset, center.y, center.z + offset), offset));
	_northeast->parent = this;
	_southwest = std::make_shared<QuadTree>(_level + 1, _max_deviations / 2, AABB<double>(vec3(center.x - offset, center.y, center.z - offset), offset));
	_southwest->parent = this;
	_southeast = std::make_shared<QuadTree>(_level + 1, _max_deviations / 2, AABB<double>(vec3(center.x + offset, center.y, center.z - offset), offset));
	_southeast->parent = this;
}
