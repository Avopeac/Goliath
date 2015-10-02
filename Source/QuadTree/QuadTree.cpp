#include "QuadTree.h"
#include "FractalHeightFunction.h"

QuadTree::QuadTree(int level, int maxlevel, const AABB<double> &box) : _box(box), _level(level), _maxlevel(maxlevel) {
	if (level < _maxlevel) {
		generate();
	}
};

QuadTree::QuadTree(const AABB<double> &box) : _box(box), _maxlevel((int)glm::log2(2.0 * box.GetExtent())) {
	generate();
};

void QuadTree::generate() {
	double size = 2.0 * _box.GetExtent();
	std::shared_ptr<FractalHeightFunction> height_func = std::make_shared<FractalHeightFunction>();
	tile = std::make_shared<Tile>();
	tile->generate(glm::pow(2.0, _level), height_func, glm::dmat4(1), glm::translate(_box.GetCenter()), glm::dmat4(size), false, true);
	_contains_mesh = true;

	std::cout << tile->_minHeight << " " << tile->_maxHeight << std::endl;
}

void QuadTree::draw(Shader &shader) {
	if (_contains_mesh) {
		tile->_tileMesh.DrawWireframe(shader);
	}
	else {
		_northwest->draw(shader);
		_northeast->draw(shader);
		_southwest->draw(shader);
		_southeast->draw(shader);
	}
}

void QuadTree::update(const Camera &camera) {

	double distance = glm::distance(camera.GetEyeVector(), _box.GetCenter());

	//std::cout << "LEVEL = " << _level << " DIST = " << distance << std::endl;

	if (!_splitted) {
		if (distance < (3189068.5) && _level < 1) {
			this->tile = nullptr;
			this->_contains_mesh = false;
			this->split();
		}
	}

	if (_northwest != nullptr)
		_northwest->update(camera);

	if (_northeast != nullptr)
		_northeast->update(camera);

	if (_southwest != nullptr)
		_southwest->update(camera);

	if (_southeast != nullptr)
		_southeast->update(camera);
}

void QuadTree::merge() {
	//Implement..
}

void QuadTree::compute_level_metric(const Camera &camera) {
	double t = 2.0 * _pixel_error / Application::GetWindowHeight();
	double a = camera.GetNear() / glm::tan(glm::radians(camera.GetFovy() * 0.5));
	double c = a / t;
	double metric = glm::max(glm::abs(this->tile->_maxHeight), glm::abs(this->tile->_minHeight)) * c;
}

void QuadTree::split() {

	//Center of the parent AABB
	glm::highp_dvec3 center = _box.GetCenter();

	//Offset from midpoint to a child quad 
	double offset = _box.GetExtent();

	//Create 4 sub-quadtrees
	_northwest = std::make_shared<QuadTree>(_level + 1, _maxlevel, AABB<double>(glm::highp_dvec3(center.x - offset, center.y, center.z + offset), offset));
	_northwest->parent = this;

	_northeast = std::make_shared<QuadTree>(_level + 1, _maxlevel, AABB<double>(glm::highp_dvec3(center.x + offset, center.y, center.z + offset), offset));
	_northeast->parent = this;

	_southwest = std::make_shared<QuadTree>(_level + 1, _maxlevel, AABB<double>(glm::highp_dvec3(center.x - offset, center.y, center.z - offset), offset));
	_southwest->parent = this;

	_southeast = std::make_shared<QuadTree>(_level + 1, _maxlevel, AABB<double>(glm::highp_dvec3(center.x + offset, center.y, center.z - offset), offset));
	_southeast->parent = this;

	_splitted = true;
}
