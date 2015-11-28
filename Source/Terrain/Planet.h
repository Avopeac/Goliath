#pragma once
#include "View/Drawable.h"
#include "Terrain/QuadTree.h"
#include "Terrain/WaterQuadTree.h"
#include "Terrain/Skybox.h"
#include "Terrain/Noise3D.h"
class Planet : public Drawable {
public:
	Planet(double radius);
	Planet(const Planet&) = delete;
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera & camera, double delta_time) override;
private:
	Noise3D noise_maker;
	std::vector<glm::vec3> gradients;
	std::vector<unsigned char> permutations;
	std::shared_ptr<Skybox> _skybox;

	std::shared_ptr<Shader> _ground_shader;
	std::shared_ptr<QuadTree> _north;
	std::shared_ptr<QuadTree> _south;
	std::shared_ptr<QuadTree> _west;
	std::shared_ptr<QuadTree> _east;
	std::shared_ptr<QuadTree> _hither;
	std::shared_ptr<QuadTree> _yon;

	std::shared_ptr<Shader> _water_shader;
	std::shared_ptr<WaterQuadTree> _north_water;
	std::shared_ptr<WaterQuadTree> _south_water;
	std::shared_ptr<WaterQuadTree> _west_water;
	std::shared_ptr<WaterQuadTree> _east_water;
	std::shared_ptr<WaterQuadTree> _hither_water;
	std::shared_ptr<WaterQuadTree> _yon_water;
	
	void setup_cube();
	void setup_skybox();
	void create_color_ramp_texture();
	GLuint _color_ramp_id;
	double _radius;
};
