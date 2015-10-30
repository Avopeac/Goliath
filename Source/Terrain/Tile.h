#pragma once
#include "..\View\Drawable.h"
#include "..\Model\Lighting.h"
class Tile : public Drawable{
public:
	Tile() {}
	virtual void draw(const Lighting &lighting, const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Lighting &lighting, const Camera & camera, double delta_time) override;

private:
	void generate_mesh(unsigned int resolution, float scale);
	glm::mat4 _model;
};