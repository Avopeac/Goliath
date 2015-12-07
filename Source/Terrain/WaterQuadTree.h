#pragma once
#include "WaterTile.h"
#include <memory>
#include <GLM/glm.hpp>
#include "View/Drawable.h"

class WaterQuadTree : public Drawable {
public:
	WaterQuadTree(const glm::dmat4& rotation, const glm::dmat4& translation, double extents, double radii, std::shared_ptr<Shader> shader);
	WaterQuadTree(const WaterQuadTree&) = delete;
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
	unsigned short _level = 0; //The current recursion level
	const unsigned short _deepest_level = 40; // The deepest level we're allowed to go in the WaterQuadTree
	const double _lod_factor = 4.0; // The higher _lod_factor the higher resolution / more lod
	WaterQuadTree *_parent = nullptr; //The parent quad tree
	std::shared_ptr<WaterTile> _patch = nullptr; //The object contained in a leaf of the quad tree
	bool _has_children = false; //The child quad trees
	std::shared_ptr<WaterQuadTree> _northwest = nullptr;
	std::shared_ptr<WaterQuadTree> _northeast = nullptr;
	std::shared_ptr<WaterQuadTree> _southwest = nullptr;
	std::shared_ptr<WaterQuadTree> _southeast = nullptr;
	//#### Inline functions for speed ####
	inline double compute_level_metric(const Camera & camera, double distance) {
		return distance - _lod_factor * _extents;
	};

	inline double distance_nearest_corner(const Camera & camera) {
		return glm::abs(glm::dot((glm::dvec3(_translation[3]) - camera.get_deye()), -glm::dvec3(camera.get_dview()[3])) - _extents);
	};

	inline double distance_to_patch(const Camera &camera, const glm::dvec3 &mid_point) {
		return glm::max(0.0, glm::distance(mid_point, camera.get_deye()));
	};
};