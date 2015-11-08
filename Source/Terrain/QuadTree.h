#pragma once
#include "Tile.h"
#include "..\View\Drawable.h"
#include <memory>
#include <GLM\glm.hpp>
class QuadTree : public Drawable {
public:
	//For root quads
	QuadTree(const glm::vec3 &center, float extents) : Drawable(), _center(center), _extents(extents) {
		create_patch();
	};

	// Inherited via Drawable
	virtual void draw(const Lighting &lighting, const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Lighting &lighting, const Camera &camera, double delta_time) override;

private:
	void subdivide();
	void create_patch();
	//The axis aligned bounding box
	glm::vec3 _center;
	float _extents;
	//The current recursion level
	unsigned int _level = 0;
	bool _has_children = false;
	bool _has_patch = false;
	//The parent quad tree
	QuadTree *_parent = nullptr;
	//The object contained in a leaf of the quad tree
	std::shared_ptr<Tile> _patch;
	//The child quad trees
	std::shared_ptr<QuadTree> _northwest = nullptr;
	std::shared_ptr<QuadTree> _northeast = nullptr;
	std::shared_ptr<QuadTree> _southwest = nullptr;
	std::shared_ptr<QuadTree> _southeast = nullptr;
};