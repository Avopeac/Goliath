#pragma once
#include "PlanetTile.h"
#include <memory>
#include <GLM/glm.hpp>
#include "View/Drawable.h"
class QuadTree : public Drawable {
public:
	QuadTree(const glm::dmat4& rotation, const glm::dmat4& translation, double extents, double radii, std::shared_ptr<Shader> shader);
	QuadTree(const QuadTree&) = delete;
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera &camera, double delta_time) override;
	bool setup_done() const { return _patch->setup_done(); }
	bool remove_children();
private:
	void subdivide();
	void create_patch();
	glm::dmat4 _translation; 
	glm::dmat4 _rotation;
	double _extents; //The axis aligned bounding box
	double _radii;
	unsigned int _level = 0; //The current recursion level
	const unsigned int _deepest_level = 32; // The deepest level we're allowed to go in the quadtree
	const double _lod_factor = 8.0; // The higher _lod_factor the higher resolution / more lod
	QuadTree *_parent = nullptr; //The parent quad tree
	std::shared_ptr<PlanetTile> _patch = nullptr; //The object contained in a leaf of the quad tree
	bool _has_children = false; //The child quad trees
	std::shared_ptr<QuadTree> _northwest = nullptr;
	std::shared_ptr<QuadTree> _northeast = nullptr;
	std::shared_ptr<QuadTree> _southwest = nullptr;
	std::shared_ptr<QuadTree> _southeast = nullptr;
	
	//#### Inline functions for speed ####
	inline double compute_level_metric(const Camera & camera, double distance) {
		return distance - _lod_factor * glm::pow(_extents, 0.98);
	};

	inline double distance_nearest_corner(const Camera & camera) {
		return glm::abs(glm::dot((glm::dvec3(_translation[3]) - camera.get_deye()), -glm::dvec3(camera.get_dview()[3])) - _extents);
	};

	inline double distance_to_patch(const Camera &camera, const glm::dvec3 &mid_point) {
		return glm::distance(mid_point, camera.get_deye());
	};

	//Morphing
	//double _morph_value = 0;
	//bool dont_morph = false;
};