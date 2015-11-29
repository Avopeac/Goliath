#include "Planet.h"
#include <GLM/gtx/transform.hpp>
#include <SOIL/SOIL.h>
#include "View/Renderer.h"
#include "View/ShaderStore.h"
#include "Model/CubeMap.h"
#include "Model/Texture2DLoader.h"
#include "Terrain/QuadTree.h"
#include "Terrain/WaterQuadTree.h"
#include "Terrain/Noise3D.h"

Planet::Planet(double radius) : Drawable(), _radius(radius) {
	setup_cube();
	setup_skybox();
	create_color_ramp_texture();
	noise_maker.initialize();
}

void Planet::setup_cube() {
	//Translation amount is half the radii
	double trans = _radius * 0.5;
	//North transforms
	glm::dmat4 north_rot(1);
	glm::dmat4 north_trans(glm::translate(glm::dvec3(0, trans, 0)));
	//South transforms
	glm::dmat4 south_rot(glm::rotate(glm::pi<double>(), glm::dvec3(0, 0, 1)));
	glm::dmat4 south_trans(glm::translate(glm::dvec3(0, -trans, 0)));
	//West transforms
	glm::dmat4 west_rot(glm::rotate(glm::half_pi<double>(), glm::dvec3(0, 0, 1)));
	glm::dmat4 west_trans(glm::translate(glm::dvec3(-trans, 0, 0)));
	//East transforms
	glm::dmat4 east_rot(glm::rotate(glm::three_over_two_pi<double>(), glm::dvec3(0, 0, 1)));
	glm::dmat4 east_trans(glm::translate(glm::dvec3(trans, 0, 0)));
	//Hither transforms
	glm::dmat4 hither_rot(glm::rotate(glm::half_pi<double>(), glm::dvec3(1, 0, 0)));
	glm::dmat4 hither_trans(glm::translate(glm::dvec3(0, 0, trans)));
	//Yon transforms
	glm::dmat4 yon_rot(glm::rotate(glm::three_over_two_pi<double>(), glm::dvec3(1, 0, 0)));
	glm::dmat4 yon_trans(glm::translate(glm::dvec3(0, 0, -trans)));
	//Set up ground terrain quad cube
	_ground_shader = ShaderStore::instance().get_shader_from_store(GROUND_SHADER_PATH);
	_north = std::make_shared<QuadTree>(north_rot, north_trans, _radius, _radius, _ground_shader);
	_south = std::make_shared<QuadTree>(south_rot, south_trans, _radius, _radius, _ground_shader);
	_west = std::make_shared<QuadTree>(west_rot, west_trans, _radius, _radius, _ground_shader);
	_east = std::make_shared<QuadTree>(east_rot, east_trans, _radius, _radius, _ground_shader);
	_hither = std::make_shared<QuadTree>(hither_rot, hither_trans, _radius, _radius, _ground_shader);
	_yon = std::make_shared<QuadTree>(yon_rot, yon_trans, _radius, _radius, _ground_shader);
	//Set up sea quad cube
	_water_shader = ShaderStore::instance().get_shader_from_store(WATER_SHADER_PATH);
	_north_water = std::make_shared<WaterQuadTree>(north_rot, north_trans, _radius, _radius, _water_shader);
	_south_water = std::make_shared<WaterQuadTree>(south_rot, south_trans, _radius, _radius, _water_shader);
	_west_water = std::make_shared<WaterQuadTree>(west_rot, west_trans, _radius, _radius, _water_shader);
	_east_water = std::make_shared<WaterQuadTree>(east_rot, east_trans, _radius, _radius, _water_shader);
	_hither_water = std::make_shared<WaterQuadTree>(hither_rot, hither_trans, _radius, _radius, _water_shader);
	_yon_water = std::make_shared<WaterQuadTree>(yon_rot, yon_trans, _radius, _radius, _water_shader);
}

void Planet::create_color_ramp_texture() {
	_color_ramp_id = Texture2DLoader::load("Images/color_ramp_terrain.png");
}

void Planet::setup_skybox() {
	//Set up skybox shader
	_skybox = std::make_shared<Skybox>("Images/skybox_space_right1.png",
		"Images/skybox_space_left2.png", "Images/skybox_space_top3.png",
		"Images/skybox_space_bottom4.png", "Images/skybox_space_front5.png",
		"Images/skybox_space_back6.png");
	_skybox->set_shader(ShaderStore::instance().get_shader_from_store(SKYBOX_SHADER_PATH));
}

void Planet::draw(const Camera & camera, double delta_time) {
	//Draw skybox
	_skybox->draw(camera, delta_time);
	//Draw terrain
	_ground_shader->use();
	//Color ramp texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _color_ramp_id);
	glUniform1i(glGetUniformLocation(_ground_shader->program, "colorRampTex"), 0);
	//Permutation texture for noise
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, noise_maker.get_permutation_texture_id());
	glUniform1i(glGetUniformLocation(_ground_shader->program, "permutationTex"), 1);
	//Gradient texture for noise
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_1D, noise_maker.get_gradient_texture_id());
	glUniform1i(glGetUniformLocation(_ground_shader->program, "gradientTex"), 2);
	//Upload uniforms
	_north->draw(camera, delta_time);
	_south->draw(camera, delta_time);
	_west->draw(camera, delta_time);
	_east->draw(camera, delta_time);
	_hither->draw(camera, delta_time);
	_yon->draw(camera, delta_time);
	//Draw sea
	_water_shader->use();
	glUniform1f(glGetUniformLocation(_water_shader->program, "time"), Application::elapsed_time);
	_north_water->draw(camera, delta_time);
	_south_water->draw(camera, delta_time);
	_west_water->draw(camera, delta_time);
	_east_water->draw(camera, delta_time);
	_hither_water->draw(camera, delta_time);
	_yon_water->draw(camera, delta_time);
}

void Planet::draw_wireframe(const Camera & camera, double delta_time) {
	draw(camera, delta_time);
}
