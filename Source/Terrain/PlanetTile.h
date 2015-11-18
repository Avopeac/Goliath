#pragma once
#include "..\View\Drawable.h"
#include "..\Model\Vertex.h"
#include "..\View\ShaderStore.h"
#include "..\Terrain\SimplePlanetHeightSampler.h"
class PlanetTile : public Drawable {
public:
	PlanetTile() : Drawable() {
		set_shader(ShaderStore::instance().get_shader_from_store(GROUND_SHADER_PATH));
	}

	void generate(const glm::mat4 & translation, const glm::mat4 & scale, const glm::mat4 & rotation);
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera & camera, double delta_time) override;
	void PlanetTile::morph_vertices(float delta_time);

private:
	struct VertexData {
		VertexData(){}
		~VertexData(){}
		Vertex vertex;
		bool edge = false;
		bool skirt = false;
		glm::vec3 parent_position; // This is used for morphing
		glm::vec3 own_position; // This is used for morphing
	};
	const int _resolution = 16;
	
	std::vector<VertexData> vertex_data;

	bool is_edge(int x, int z);
	static SimplePlanetHeightSampler sampler;
	void PlanetTile::set_parent_position(int x, int z, glm::mat4 transform);
};
