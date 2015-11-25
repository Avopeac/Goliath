#pragma once
#include "PlanetTile.h"
#include <memory>
#include <GLM\glm.hpp>
#include "View/Drawable.h"

class QuadTree : public Drawable {
public:
	QuadTree();
	QuadTree(const glm::dmat4& rotation, const glm::dmat4& translation, double extents);
	QuadTree(const glm::dmat4& rotation, const glm::dmat4& translation, double extents, std::shared_ptr<Shader> shader);
	QuadTree(const QuadTree&) = delete;

	void set_rotation(const glm::dmat4 &rotation) { _rotation = rotation; }
	void set_translation(const glm::dmat4 &translation) { _translation = translation; }
	void set_extents(double extents) { _extents = extents; }

	// Inherited via Drawable
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera &camera, double delta_time) override;

	bool setup_done() const { return _patch->setup_done(); }
	
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
	
	//The current recursion level
	unsigned int _level = 0;
	// The deepest level we're allowed to go in the quadtree
	const unsigned int _deepest_level = 100;
	// The higher _lod_factor the higher resolution / more lod
	const double _lod_factor = 16.0;

	//The parent quad tree
	QuadTree *_parent = nullptr;

	//The object contained in a leaf of the quad tree
	std::shared_ptr<PlanetTile> _patch = nullptr;

	//The child quad trees
	bool _has_children = false;
	std::shared_ptr<QuadTree> _northwest = nullptr;
	std::shared_ptr<QuadTree> _northeast = nullptr;
	std::shared_ptr<QuadTree> _southwest = nullptr;
	std::shared_ptr<QuadTree> _southeast = nullptr;

	//Morphing
	double _morph_value = 0;
	bool dont_morph = false;
};