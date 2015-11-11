#pragma once
#include "..\View\Drawable.h"
#include "..\Model\Lighting.h"
#include <GLM/gtx/transform.hpp>

class Tile : public Drawable{
public:
	Tile(unsigned int resolution, const glm::mat4 &scale, const glm::mat4 &translation, const glm::mat4 &rotation);
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera & camera, double delta_time) override;
	void generate_mesh();
	void generate_normalized_mesh();
private:
	void generate_vertex(glm::vec3 position);
	void generate_skirt_vertex(glm::vec3 position);
	void generate_vertex_helper(float offset, float step, unsigned int column, bool edge);
	void generate_normalized_vertex(glm::vec3 position);
	void generate_normalized_skirt_vertex(glm::vec3 position);
	void generate_normalized_vertex_helper(float offset, float step, unsigned int column, bool edge);
	void setup_draw(const Camera & camera, double delta_time);
	
	const unsigned int _skirt_padding = 2;
	const float _skirt_offset = -0.10f;
	const float _skirt_ratio = 0.95f;
	unsigned int _resolution;
	glm::mat4 _premult_transf;
	glm::mat4 _scale;
	glm::mat4 _rotation;
	glm::mat4 _translation;
};