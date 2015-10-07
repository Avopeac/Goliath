#pragma once
#include <memory>
#include <thread>
#include <GLM\glm.hpp>
#include "../Math/AABB.h"
#include "../Visibility/Camera.h"
#include "Tile.h"

#define DEFAULT_TILE_SIZE 32
#define DEFAULT_SCREEN_SPACE_ERROR 4
#define DEFAULT_DEVATIONS_ZERO 16384

class QuadTree {
public:
	
	QuadTree(int level, int maxlevel, const AABB<double> &box);
	QuadTree(const AABB<double> &box);
	~QuadTree() = default;
	const AABB<double> &getBoundingBox() const { return _box; }
	void draw(Shader &shader, const Camera &camera);

private:
	
	typedef glm::highp_dvec3 vec3;

	void merge();
	void split();
	void generate();
	bool has_children();
	bool has_leaf();
	bool child_data_resident();
	double compute_level_metric(const Camera &camera, double distance);
	double distance_nearest_corner(const Camera &camera);

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

	const int _max_deviations = 0;
	static int _id_counter;
	int _level = 0;
	int _this_id;
	
	//Terrain data
	std::shared_ptr<Tile> _tile;
};