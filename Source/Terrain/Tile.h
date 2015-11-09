#pragma once
#include "..\View\Drawable.h"
#include "..\Model\Lighting.h"
#include <GLM/gtx/transform.hpp>

class Tile : public Drawable{
public:
	Tile(unsigned int resolution, const glm::mat4 &scale, const glm::mat4 &translation, const glm::mat4 &rotation, bool normalize);
	virtual void draw(const Lighting &lighting, const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Lighting &lighting, const Camera & camera, double delta_time) override;

private:
	void generate_vertex(glm::vec3 position);
	void generate_vertex_skirt(glm::vec3 position, glm::vec3 normal);
	void setup_draw(const Lighting &lighting, const Camera & camera, double delta_time);
	void translateTile(const glm::vec3 & t);
	unsigned int skirt_padding();
	void generate_mesh();
	unsigned int _resolution;
	glm::mat4 _scale;
	glm::mat4 _rotation;
	glm::mat4 _translation;
	bool _normalize;
};