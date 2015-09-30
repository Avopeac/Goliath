#pragma once
#include <memory>
#include "../Math/AABB.h"
#include "../Visibility/Camera.h"
#include "Tile.h"

class QuadTree {
public:
	
	QuadTree(const AABB<double> &box) : _box(box) {}
	~QuadTree() = default;

	const AABB<double> &getBoundingBox() const { return _box; }

	void update(const Camera &camera);

private:

	void merge();
	void split();

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
	
	//Terrain data
	Tile tile;
};