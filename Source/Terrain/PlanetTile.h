#pragma once
#include "..\View\Drawable.h"
#include "..\Model\Vertex.h"
#include "..\View\ShaderStore.h"
#include "..\Terrain\SimplePlanetHeightSampler.h"
class PlanetTile : public Drawable {
public:
	PlanetTile(const glm::dmat4 &translation, const glm::dmat4 &scale, const glm::dmat4 &rotation, double radii);
	PlanetTile(const glm::dmat4 &translation, const glm::dmat4 &scale, const glm::dmat4 &rotation, double radii, std::shared_ptr<Shader> shader);
	PlanetTile(const PlanetTile&) = delete;

	void generate();
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera & camera, double delta_time) override;
	void PlanetTile::morph_vertices(double delta_time);
	const glm::dvec3 &get_maximum() const { return _max; }
	const glm::dvec3 &get_minimum() const { return _min; }
	const glm::dvec3 &get_center() const { return _center; }
	const glm::dvec3 &get_extents() const { return _extents; }
	bool setup_done() const { return _setup_done; }

private:
	class PlanetTileMessage;
	class VertexData;

	std::vector<VertexData> vertex_data;
	const int _resolution = 16;
	bool _setup_done = false;
	int _message_ref = -1;

	glm::dvec3 _center;
	glm::dvec3 _extents;
	glm::dvec3 _min;
	glm::dvec3 _max;
	glm::dmat4 _translation;
	glm::dmat4 _scale;
	glm::dmat4 _rotation;
	double _radii;
	
	void predraw(const Camera &camera);
	bool is_edge(int x, int z);

	static SimplePlanetHeightSampler sampler;
	
	void set_parent_position(int x, int z, const glm::dmat4 &transform);
	inline double height_scaler(const glm::dvec3 &pos) { 
		double h = sampler.sample(pos) - 1.8;
		return _radii + h * 8000.0; //Max is approx 8000 units
	}
};
