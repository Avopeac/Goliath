#pragma once
#include "..\View\Drawable.h"
#include "..\Model\Lighting.h"
#include <GLM/gtx/transform.hpp>

class Tile : public Drawable{
public:
	Tile();
	Tile(unsigned int resolution, const glm::mat4 &scale, const glm::mat4 &translation, const glm::mat4 &rotation);
	void setup(unsigned int resolution, const glm::mat4 & scale, const glm::mat4 & translation, const glm::mat4 & rotation);
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera & camera, double delta_time) override;
	void generate_mesh();
	void upload_mesh();
private:

	void generate_vertex(glm::vec3 position);
	float get_height(const glm::vec3 &position, float radius, float scale, float octaves, float lacunarity, float dimension);
	glm::vec3 get_color_ramp(float min, float max, float height);
	glm::vec3 get_central_difference_normal(const glm::vec3 & position);
	glm::vec3 get_forward_difference_normal(const glm::vec3 & position);
	void generate_skirt_vertex(glm::vec3 position);
	void generate_vertex_helper(unsigned int column, bool edge);
	void setup_draw(const Camera & camera, double delta_time);
	
	const unsigned int _skirt_padding = 2;
	const float _skirt_offset = -0.10f;
	const float _skirt_ratio = 0.95f;
	unsigned int _resolution;
	float _inverse_resolution;
	const float offset = 0.5f;
	glm::mat4 _premult_transf;
	glm::mat4 _scale;
	glm::mat4 _rotation;
	glm::mat4 _translation;
};