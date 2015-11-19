#pragma once
#include "..\View\Drawable.h"
#include "..\Terrain\QuadTree.h"
#include "..\Terrain\Skybox.h"
#include "..\View\ShaderStore.h"
#include "..\Model\CubeMap.h"
#include <SOIL\SOIL.h>
#include <GLM\gtx\transform.hpp>
class Planet : public Drawable {
public:
	Planet(double radius) : Drawable(), _radius(radius){
		setup_cube();
		setup_terrain();
		setup_skybox();
		create_color_ramp_texture();
		create_gradient_array();
		create_permutation_array();
		create_permutation_texture();
	}
	// Inherited via Drawable
	virtual void draw(const Camera & camera, double delta_time) override;
	virtual void draw_wireframe(const Camera & camera, double delta_time) override;
private:
	std::vector<glm::vec3> gradients;
	std::vector<unsigned char> permutations;
	std::shared_ptr<Skybox> _skybox;
	std::shared_ptr<QuadTree> _north;
	std::shared_ptr<QuadTree> _south;
	std::shared_ptr<QuadTree> _west;
	std::shared_ptr<QuadTree> _east;
	std::shared_ptr<QuadTree> _hither;
	std::shared_ptr<QuadTree> _yon;
	std::shared_ptr<Shader> _ground_shader;
	void setup_cube();
	void setup_terrain();
	void setup_skybox();
	void create_color_ramp_texture();
	void create_gradient_array();
	void create_permutation_array();
	void create_permutation_texture();
	GLuint _color_ramp_id;
	GLuint _permutation_id;
	float _radius;
};
