#include <thread>
#include <limits>
#include <future>
#include "QuadTree.h"
#include "FractalHeightFunction.h"

int QuadTree::_id_counter = 0;

std::shared_ptr<HeightFunction> QuadTree::height_function = std::make_shared<FractalHeightFunction>();

QuadTree::QuadTree(int level, int max_deviations, const AABB<double> &box) : _box(box), _level(level), _max_deviations(max_deviations) {
	_this_id = _id_counter++;
	//_thread = std::thread(&QuadTree::generate_threaded, this);
	generate_threaded();
	//_tile->_tileMesh.SetupMesh();
};

QuadTree::QuadTree(const AABB<double> &box) : QuadTree(0, DEFAULT_DEVATIONS_ZERO, box) {};

void QuadTree::generate() {
	_tile = std::make_shared<Tile>();
	glm::dmat4 rotation(1);
	glm::dmat4 scale(2.0 * _box.GetExtent());
	_tile->generate(DEFAULT_TILE_SIZE, rotation, _box.GetCenter(), scale, false, true);
}

void QuadTree::generate_threaded() {
	_thread_work_complete = false;
	_tile = std::make_shared<Tile>();
	glm::dmat4 rotation(1);
	glm::dmat4 scale(2.0 * _box.GetExtent());
	_tile->generate(DEFAULT_TILE_SIZE, height_function, rotation, _box.GetCenter(), scale, false, true);
	_thread_work_complete = true;
	_tile_setup_complete = false;
}

bool QuadTree::has_children() {
	return _northwest != nullptr && _northeast != nullptr && _southwest != nullptr && _southeast != nullptr;
}

bool QuadTree::has_leaf() {
	return _tile != nullptr;
}

bool QuadTree::child_data_resident() {
	//return _northwest->has_leaf() && _northeast->has_leaf() && _southwest->has_leaf() && _southeast->has_leaf();

	return (_northwest->_thread_work_complete) &&
		(_northeast->_thread_work_complete) &&
		(_southwest->_thread_work_complete) &&
		(_southeast->_thread_work_complete);
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

	return glm::dot(min, min);
}

void QuadTree::draw(Shader &shader, Camera &camera) {

	//Frustum &frustum = camera.GetFrustum();
	//frustum.DebugDraw(shader);

	if (_thread_work_complete && !_tile_setup_complete) {
		_tile->_tileMesh.SetupMesh();
		_tile_setup_complete = true;
	}

	double rho = compute_level_metric(camera, distance_nearest_corner(camera));

	bool threshold_met = rho < DEFAULT_SCREEN_SPACE_ERROR;
	bool waiting_for_children = has_children() && !child_data_resident();

	if (threshold_met || waiting_for_children) { 

		if (threshold_met && has_children() && !waiting_for_children) {
			merge();
		}

		if (has_leaf()) {
			_tile->_tileMesh.DrawWireframe(shader);
		}
		else {
			generate_threaded();
		}
	}
	else {

		if (has_children()) {
			_northwest->draw(shader, camera);
			_northeast->draw(shader, camera);
			_southwest->draw(shader, camera);
			_southeast->draw(shader, camera);

			if (child_data_resident() && has_leaf()) {
				_tile = nullptr;
			}
		}
		else {
			if (_thread_work_complete && _tile_setup_complete) {
				split();
			}
		}
	}
}

void QuadTree::merge() {

	if (has_children()) {
		if (!_northwest->has_children() && !_northeast->has_children() && !_southwest->has_children() && !_southeast->has_children()) {
			_northwest->_tile = nullptr;
			_northwest->parent = nullptr;
			_northwest = nullptr;
			_northeast->_tile = nullptr;
			_northeast->parent = nullptr;
			_northeast = nullptr;
			_southwest->_tile = nullptr;
			_southwest->parent = nullptr;
			_southwest = nullptr;
			_southeast->_tile = nullptr;
			_southeast->parent = nullptr;
			_southeast = nullptr;
			
		}
		else {
			_northwest->merge();
			_northeast->merge();
			_southwest->merge();
			_southeast->merge();
		}
	}
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
