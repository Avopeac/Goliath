#pragma once
#include "PlanetTile.h"
#include "..\View\Drawable.h"
#include <memory>
#include <GLM\glm.hpp>

class QuadTree : public Drawable {
public:
	QuadTree();
	QuadTree(const glm::mat4& rotation, const glm::mat4& translation, float extents);
	QuadTree(const glm::mat4& rotation, const glm::mat4& translation, float extents, std::shared_ptr<Shader> shader);

	QuadTree(const QuadTree&) = delete;

	void set_rotation(const glm::mat4 &rotation) { _rotation = rotation; }
	void set_translation(const glm::mat4 &translation) { _translation = translation; }
	void set_extents(float extents) { _extents = extents; }

	// Inherited via Drawable
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera &camera, double delta_time) override;

	bool setup_done() const { return _has_patch && _patch->setup_done(); }
	
private:
	void subdivide();
	void create_patch();
	double compute_level_metric(const Camera & camera, double distance);
	double distance_nearest_corner(const Camera & camera);
	double distance_to_patch(const Camera & camera, glm::vec3 mid_point);
	// Threshold for LOD error metric
	const float _TAU = 0.0;
	// The deepest level we're allowed to go in the quadtree
	const int _DEEPEST_LEVEL = 20;
	//The axis aligned bounding box
	glm::mat4 _translation;
	glm::mat4 _rotation;
	float _extents; // Needs default ?
	//The current recursion level
	unsigned int _level = 0;
	bool _has_children = false;
	bool _has_patch = false;
	double _screen_space_error = 0.05f;
	//The parent quad tree
	QuadTree *_parent = nullptr;
	//The object contained in a leaf of the quad tree
	std::shared_ptr<PlanetTile> _patch = nullptr;
	//The child quad trees
	std::shared_ptr<QuadTree> _northwest = nullptr;
	std::shared_ptr<QuadTree> _northeast = nullptr;
	std::shared_ptr<QuadTree> _southwest = nullptr;
	std::shared_ptr<QuadTree> _southeast = nullptr;

	float _morph_value = 0;
	bool dont_morph = false;
};