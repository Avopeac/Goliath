#pragma once
#include "Tile.h"
#include "..\View\Camera.h"
class QuadTree {
public:
	QuadTree(const glm::mat4 &scale, const glm::mat4 &translation, const glm::mat4 &rotation);

private:
	void draw(const Camera &camera, float delta_time);
	void create_children();

	std::shared_ptr<QuadTree> _nw;
	std::shared_ptr<QuadTree> _ne;
	std::shared_ptr<QuadTree> _sw;
	std::shared_ptr<QuadTree> _se;
	std::shared_ptr<Tile> _tile;
	float _screen_space_error = 5.0f;

	//Built in AABB
	glm::vec3 _origin;
	glm::vec3 _extents;
};