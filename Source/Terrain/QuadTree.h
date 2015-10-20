#pragma once
#include <memory>
#include <thread>
#include <GLM\glm.hpp>
#include "../Math/AABB.h"
#include "../Visibility/Camera.h"
#include "Tile.h"

#define DEFAULT_TILE_SIZE 32
#define DEFAULT_SCREEN_SPACE_ERROR 4
#define DEFAULT_DEVATIONS_ZERO 32768

class QuadTree {
public:
	
	QuadTree(int level, int maxlevel, const AABB<double> &box);
	QuadTree(const AABB<double> &box);
	~QuadTree() = default;
	const AABB<double> &getBoundingBox() const { return _box; }
	void draw(Shader &shader, Camera &camera);

private:

	enum class Orientation {
		NORTHWEST, NORTHEAST,
		SOUTHWEST, SOUTHEAST,
		ROOT,
	};
	
	typedef glm::highp_dvec3 vec3;

	void merge();
	void split();
	void draw_children(Shader &shader, Camera &camera);
	void draw_tile(Shader &shader);
	bool has_children();
	bool child_data_resident();
	double compute_level_metric(const Camera &camera, double distance);
	double distance_nearest_corner(const Camera &camera);

	const AABB<double> _box;
	std::vector<vec3> _aabb_points;
	
	Orientation _orientation;

	//Parent
	QuadTree *_parent = nullptr;

	//Children
	std::shared_ptr<QuadTree> _northwest = nullptr;
	std::shared_ptr<QuadTree> _northeast = nullptr;
	std::shared_ptr<QuadTree> _southwest = nullptr;
	std::shared_ptr<QuadTree> _southeast = nullptr;

	const int _max_deviations = 0;
	static int _id_counter;
	int _level = 0;
	int _this_id;

	std::thread _thread;
	bool _thread_work_complete = false;
	bool _thread_working = false;
	
	int _tile_id = -1;
	bool _tile_setup_complete = false;
	std::shared_ptr<Tile> _tile = nullptr;
	static std::shared_ptr<HeightFunction> _height_function;
};