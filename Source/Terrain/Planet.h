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

	inline static bool horizon_culling(const glm::dvec3 &v, const glm::dvec3 &o, double ro, const glm::dvec3 &b, double rb) {
		double ob_sqr(glm::distance2(o,b));
		double vo_sqr(glm::distance2(v, o));
		double vba_sqr(glm::distance2(v, b));
		double rorb_sqr(glm::pow(ro - rb, 2.0));
		double vh(glm::sqrt(vo_sqr - glm::pow(ro, 2.0)));
		double hy(glm::sqrt(ob_sqr - rorb_sqr));
		double vbe_sqr(glm::pow(vh + hy, 2.0) + glm::pow(rb, 2.0));
		//if (ob_sqr < rorb_sqr) {
		//return true;
		//}
		return vba_sqr > vbe_sqr;
	}

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

	//std::shared_ptr<Shader> _water_shader;
	//std::shared_ptr<WaterQuadTree> _north_water;
	//std::shared_ptr<WaterQuadTree> _south_water;
	//std::shared_ptr<WaterQuadTree> _west_water;
	//std::shared_ptr<WaterQuadTree> _east_water;
	//std::shared_ptr<WaterQuadTree> _hither_water;
	//std::shared_ptr<WaterQuadTree> _yon_water;
	
	void setup_cube();
	void setup_skybox();
	void create_color_ramp_texture();
	GLuint _color_ramp_id;
	GLuint _ground_id;
	GLuint _ground_n_id;
	GLuint _grass_id;
	GLuint _grass_n_id;
	GLuint _rock_id;
	GLuint _rock_n_id;
	GLuint _forest_id;
	//GLuint _water_n_id;
	double _radius;
};
