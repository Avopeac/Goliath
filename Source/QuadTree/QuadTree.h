#pragma once
#include "BoundingBox.h"
class QuadTree {
public:
	
	QuadTree(const BoundingBox &box) : _box(box) {}
	~QuadTree() = default;

	const BoundingBox &getBoundingBox() const { return _box; }

private:

	void divide();
	const BoundingBox _box;
};