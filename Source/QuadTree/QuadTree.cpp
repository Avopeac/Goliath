#include <thread>
#include <limits>
#include <future>
#include "QuadTree.h"
#include "FractalHeightFunction.h"
#include "..\MultiThreading\MessageSystem.h"
#include "..\MultiThreading\TileMessage.h"

//Static variables
int QuadTree::_id_counter = 0;
std::shared_ptr<HeightFunction> QuadTree::height_function = std::make_shared<FractalHeightFunction>();

QuadTree::QuadTree(const AABB<double> &box) : QuadTree(0, DEFAULT_DEVATIONS_ZERO, box) {};
QuadTree::QuadTree(int level, int max_deviations, const AABB<double> &box) : _box(box), _level(level), _max_deviations(max_deviations) {
	_this_id = _id_counter++;

	//Add points in the AABB to compare distance
	vec3 c = _box.get_center();
	double o = _box.get_extent();
	_aabb_points.push_back(c);
	_aabb_points.push_back(vec3(c.x - o, c.y, c.z + o));
	_aabb_points.push_back(vec3(c.x + o, c.y, c.z + o));
	_aabb_points.push_back(vec3(c.x - o, c.y, c.z - o));
	_aabb_points.push_back(vec3(c.x + o, c.y, c.z - o));

};

bool QuadTree::has_children() {
	return _northwest != nullptr &&
		_northeast != nullptr &&
		_southwest != nullptr &&
		_southeast != nullptr;
}

double QuadTree::distance_nearest_corner(const Camera &camera) {
	double max = std::numeric_limits<double>().max();
	double min_distance = max;
	vec3 min(max, max, max);
	for (auto it = _aabb_points.begin(); it != _aabb_points.end(); ++it) {
		vec3 elem = camera.get_eye_vector() - (*it);
		if (glm::dot(elem, elem) < glm::dot(min, min)) {
			min = elem;
		}
	}
	return glm::dot(min, min);
}

bool QuadTree::child_data_resident() {
	return ((_northwest->_tile_id != -1 || _northwest->_tile_setup_complete) &&
		(_northeast->_tile_id != -1 || _northeast->_tile_setup_complete) &&
		(_southwest->_tile_id != -1 || _southwest->_tile_setup_complete) &&
		(_southeast->_tile_id != -1 || _southeast->_tile_setup_complete));
}

void QuadTree::draw(Shader &shader, Camera &camera) {
	double rho = compute_level_metric(camera, distance_nearest_corner(camera));
	bool threshold_met = rho < DEFAULT_SCREEN_SPACE_ERROR;
	bool building_children = has_children() && !child_data_resident();

	if (building_children) {
		draw_children(shader, camera);
	}

	if (threshold_met || building_children) {
		draw_tile(shader);
	}
	else if (has_children()) {
		draw_children(shader, camera);
	}
	else {
		split();
	}
}

void QuadTree::draw_children(Shader &shader, Camera &camera) {
	_northwest->draw(shader, camera);
	_northeast->draw(shader, camera);
	_southwest->draw(shader, camera);
	_southeast->draw(shader, camera);
}

void QuadTree::draw_tile(Shader &shader) {

	if (_tile_id != -1) {
		std::shared_ptr<Message> message = MessageSystem::instance().get(_tile_id);
		if (message != nullptr) {
			TileMessage *tile_message = dynamic_cast<TileMessage*>(message.get());
			_tile = tile_message->get_tile();
			_tile->_mesh.setup_mesh();
			_tile_setup_complete = true;
		}
	}

	if (_tile != nullptr && _tile_setup_complete) {
		_tile->_mesh.draw(shader);
	}
	else if (_tile_id == -1) {
		_tile_id = MessageSystem::instance().post(
			std::make_shared<TileMessage>(DEFAULT_TILE_SIZE, std::make_shared<FractalHeightFunction>(), glm::dmat4(1), _box.get_center(), glm::dmat4(2.0 * _box.get_extent()), false, true)
			);
	}
}

void QuadTree::merge() {

}

double QuadTree::compute_level_metric(const Camera &camera, double distance) {
	double kappa = Application::get_window_width() / (2.0 * glm::tan(glm::radians(camera.get_fov_x() * 0.5)));
	double rho = (_max_deviations * _max_deviations * kappa * kappa) / distance;
	return rho;
}

void QuadTree::split() {

	vec3 center = _box.get_center();
	double offset = 0.5 * _box.get_extent();

	_northwest = std::make_shared<QuadTree>(_level + 1, _max_deviations / 2, AABB<double>(vec3(center.x - offset, center.y, center.z + offset), offset));
	_northwest->parent = this;

	_northeast = std::make_shared<QuadTree>(_level + 1, _max_deviations / 2, AABB<double>(vec3(center.x + offset, center.y, center.z + offset), offset));
	_northeast->parent = this;

	_southwest = std::make_shared<QuadTree>(_level + 1, _max_deviations / 2, AABB<double>(vec3(center.x - offset, center.y, center.z - offset), offset));
	_southwest->parent = this;

	_southeast = std::make_shared<QuadTree>(_level + 1, _max_deviations / 2, AABB<double>(vec3(center.x + offset, center.y, center.z - offset), offset));
	_southeast->parent = this;
}
