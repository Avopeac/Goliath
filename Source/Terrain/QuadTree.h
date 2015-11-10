#pragma once
#include "Tile.h"
#include "..\View\Drawable.h"
#include <memory>
#include <GLM\glm.hpp>
class QuadTree : public Drawable {
public:
	QuadTree(const glm::mat4 &rotation, const glm::mat4 &translation, float extents) : Drawable(), _rotation(rotation), _translation(translation), _extents(extents) {
		_origin = glm::vec3(_translation[3]);
	};

	// Inherited via Drawable
	virtual void draw(const Lighting &lighting, const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Lighting &lighting, const Camera &camera, double delta_time) override;

private:
	void subdivide();
	void create_patch();
	float compute_level_metric(const Camera & camera, float distance);
	float distance_nearest_corner(const Camera & camera);
	//The axis aligned bounding box
	glm::mat4 _translation;
	glm::mat4 _rotation;
	glm::vec3 _origin;
	float _extents;
	//The current recursion level
	unsigned int _level = 0;
	bool _has_children = false;
	bool _has_patch = false;
	float _screen_space_error = 0.05f;
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