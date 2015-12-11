#pragma once
#include "View/Drawable.h"
#include "Model/Vertex.h"
#include "View/ShaderStore.h"
#include "Terrain/SimplePlanetHeightSampler.h"

/// This may not be very pretty but keeps memory footprint down for instances of this class
#ifndef PLANET_TILE_MAX_MOUNTAIN_HEIGHT 
#define PLANET_TILE_MAX_MOUNTAIN_HEIGHT 81920
#endif
#ifndef PLANET_TILE_MAX_MOUNTAIN_INV_HEIGHT 
#define PLANET_TILE_MAX_MOUNTAIN_INV_HEIGHT 0.00136533333
#endif
#ifndef PLANET_TILE_RESOLUTION
#define PLANET_TILE_RESOLUTION 16
#endif
#ifndef PLANET_TILE_INV_RESOLUTION
#define PLANET_TILE_INV_RESOLUTION 0.0625
#endif
#ifndef PLANET_TILE_OFFSET 
#define PLANET_TILE_OFFSET 0.5
#endif

//There are many tiles created at runtime, this class should be kept as small as possible
class PlanetTile : public Drawable {
public:
	~PlanetTile();
	PlanetTile(const glm::dmat4 &translation, const glm::dmat4 &scale, const glm::dmat4 &rotation, double radii, std::shared_ptr<Shader> shader);
	PlanetTile(const PlanetTile&) = delete;
	void generate();
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera & camera, double delta_time) override;
	const glm::dvec3 &get_center() const { return _center; }
	const glm::dvec3 &get_extents() const { return _extents; }
	bool setup_done() const { return _setup_done; }

private:
	class PlanetTileMessage;
	class VertexData;
	static SimplePlanetHeightSampler sampler;

	//Tile setup data
	bool _setup_done = false;
	int _message_ref = -1;

	//Tile transformation data
	glm::dvec3 _center, _extents;
	glm::dmat4 _transform;
	const double _radii;
	const double _inv_outer_radii;

	///Gets a noise value by giving position, scales this according to max mountain height
	inline double height_scaler(const glm::dvec3 &pos) { 
		return sampler.sample(pos);
	}

	///Is the given tile index on the edge?
	inline bool is_edge(int x, int z) {
		return (x == -1) || (z == -1) || (x == PLANET_TILE_RESOLUTION + 1) || (z == PLANET_TILE_RESOLUTION + 1);
	}

	void predraw(const Camera &camera);
};
