#pragma once
#include <memory>
#include <GLM\glm.hpp>
#include "../Math/AABB.h"
#include "../Visibility/Camera.h"
#include "Tile.h"

class QuadTree {
public:
	
	QuadTree(int level, int maxlevel, const AABB<double> &box);
	QuadTree(const AABB<double> &box);
	~QuadTree() = default;
	const AABB<double> &getBoundingBox() const { return _box; }
	void update(const Camera &camera);
	void draw(Shader &shader);

private:

	void merge();
	void split();
	void generate();
	void compute_level_metric(const Camera &camera);

	const AABB<double> _box;
	
	//Neighbours
	std::shared_ptr<QuadTree> _north;
	std::shared_ptr<QuadTree> _east;
	std::shared_ptr<QuadTree> _south;
	std::shared_ptr<QuadTree> _west;

	//Parent
	QuadTree *parent = nullptr;

	//Children
	std::shared_ptr<QuadTree> _northwest;
	std::shared_ptr<QuadTree> _northeast;
	std::shared_ptr<QuadTree> _southwest;
	std::shared_ptr<QuadTree> _southeast;

	const int _maxlevel = 0;
	int _level = 0;
	bool _splitted = false;
	bool _contains_mesh = false;

	const unsigned int _pixel_error = 4; //4 pixels is too noticable of a screen space error
	
	//Terrain data
	std::shared_ptr<Tile> tile;
};