#pragma once
#include "..\View\Drawable.h"
#include "..\Model\Vertex.h"
#include "..\View\ShaderStore.h"
#include "..\Terrain\SimplePlanetHeightSampler.h"
class PlanetTile : public Drawable {
public:
	PlanetTile(const glm::mat4 &translation, const glm::mat4 &scale, const glm::mat4 &rotation);
	PlanetTile(const glm::mat4 &translation, const glm::mat4 &scale, const glm::mat4 &rotation, std::shared_ptr<Shader> shader);

	PlanetTile(const PlanetTile&) = delete;

	void generate();
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera & camera, double delta_time) override;
	void PlanetTile::morph_vertices(float delta_time);


	bool setup_done() const { return _setup_done; }

private:
	class PlanetTileMessage;
	class VertexData;

	std::vector<VertexData> vertex_data;

	const int _resolution = 64;
	bool _setup_done = false;
	int _message_ref = -1;

	glm::vec3 _center;
	glm::mat4 _translation;
	glm::mat4 _scale;
	glm::mat4 _rotation;
	
	void predraw(const Camera &camera);

	bool is_edge(int x, int z);
	static SimplePlanetHeightSampler sampler;
	void PlanetTile::set_parent_position(int x, int z, const glm::mat4 &transform);
};
