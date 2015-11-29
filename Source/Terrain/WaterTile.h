#pragma once
#include "View/Drawable.h"
#include "Model/Vertex.h"
#include "View/ShaderStore.h"

/// This may not be very pretty but keeps memory footprint down for instances of this class
#ifndef WATER_TILE_RESOLUTION
#define WATER_TILE_RESOLUTION 4
#endif
#ifndef WATER_TILE_INV_RESOLUTION
#define WATER_TILE_INV_RESOLUTION 0.25
#endif
#ifndef WATER_TILE_OFFSET 
#define WATER_TILE_OFFSET 0.5
#endif

//There are many tiles created at runtime, this class should be kept as small as possible
class WaterTile : public Drawable {
public:
	WaterTile(const glm::dmat4 &translation, const glm::dmat4 &scale, const glm::dmat4 &rotation, double radii, std::shared_ptr<Shader> shader);
	WaterTile(const WaterTile&) = delete;
	void generate();
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera & camera, double delta_time) override;
	const glm::dvec3 &get_center() const { return _center; }
	const glm::dvec3 &get_extents() const { return _extents; }
	bool setup_done() const { return _setup_done; }
private:
	class WaterTileMessage;
	class VertexData;
	//Tile setup data
	bool _setup_done = false;
	int _message_ref = -1;
	//Tile transformation data
	glm::dvec3 _center, _extents;
	glm::dmat4 _transform;
	const double _radii;
	//Is the given tile index on the edge?
	inline bool is_edge(int x, int z) {
		return (x == -1) || (z == -1) || (x == WATER_TILE_RESOLUTION + 1) || (z == WATER_TILE_RESOLUTION + 1);
	}
	//Set up before drawing
	void predraw(const Camera &camera);
};
