#pragma once
#include "..\View\Drawable.h"
#include "..\Terrain\QuadTree.h"
#include "Terrain/Skybox.h"

class Planet : public Drawable {
public:
	Planet(double radius);
	
	Planet(const Planet&) = delete;

	// Inherited via Drawable
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera & camera, double delta_time) override;

private:
	std::shared_ptr<Skybox> _skybox;
	std::shared_ptr<QuadTree> _north;
	std::shared_ptr<QuadTree> _south;
	std::shared_ptr<QuadTree> _west;
	std::shared_ptr<QuadTree> _east;
	std::shared_ptr<QuadTree> _hither;
	std::shared_ptr<QuadTree> _yon;
	std::shared_ptr<Shader> _ground_shader;

	GLuint _color_ramp_id;

};
