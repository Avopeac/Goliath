#pragma once
#include "..\View\Drawable.h"
#include "..\Model\Lighting.h"
class Tile : public Drawable{
public:
	Tile(unsigned int resolution, const glm::mat4 &scale, const glm::vec3 &translation, const glm::mat4 &rotation);
	virtual void draw(const Lighting &lighting, const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Lighting &lighting, const Camera & camera, double delta_time) override;

private:
	void generate_mesh();
	unsigned int _resolution;
	glm::mat3 _model;
	glm::vec3 _translation;
};