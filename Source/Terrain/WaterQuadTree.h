#pragma once
#include "WaterTile.h"
#include <memory>
#include <GLM/glm.hpp>
#include "View/Drawable.h"

class WaterQuadTree : public Drawable {
public:
	WaterQuadTree();
	WaterQuadTree(const glm::dmat4& rotation, const glm::dmat4& translation, double extents, double radii);
	WaterQuadTree(const glm::dmat4& rotation, const glm::dmat4& translation, double extents, double radii, std::shared_ptr<Shader> shader);
	WaterQuadTree(const WaterQuadTree&) = delete;

	// Inherited via Drawable
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera &camera, double delta_time) override;

	bool setup_done() const { return _patch->setup_done(); }
	bool remove_children();

private:
	void subdivide();
	void create_patch();
	double compute_level_metric(const Camera & camera, double distance);
	double distance_nearest_corner(const Camera & camera);
	double distance_to_patch(const Camera &camera, glm::dvec3 mid_point);


	//The axis aligned bounding box
	glm::dmat4 _translation;
	glm::dmat4 _rotation;
	double _extents; // Needs default ?
	double _radii; // Whole planet radii
	
	//The current recursion level
	unsigned int _level = 0;
	// The deepest level we're allowed to go in the WaterQuadTree
	const unsigned int _deepest_level = 100;
	// The higher _lod_factor the higher resolution / more lod
	const double _lod_factor = 16.0;

	//The parent quad tree
	WaterQuadTree *_parent = nullptr;

	//The object contained in a leaf of the quad tree
	std::shared_ptr<WaterTile> _patch = nullptr;

	//The child quad trees
	bool _has_children = false;
	std::shared_ptr<WaterQuadTree> _northwest = nullptr;
	std::shared_ptr<WaterQuadTree> _northeast = nullptr;
	std::shared_ptr<WaterQuadTree> _southwest = nullptr;
	std::shared_ptr<WaterQuadTree> _southeast = nullptr;

	//Morphing
	double _morph_value = 0;
	bool dont_morph = false;
};