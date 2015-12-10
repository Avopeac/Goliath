#include "Planet.h"

#include <GLM/gtx/transform.hpp>
#include "View/ShaderStore.h"
#include "Model/Texture2DLoader.h"
#include "Terrain/QuadTree.h"
#include "Terrain/Noise3D.h"
#include "Terrain/Water.h"
#include "Terrain/DayNight.h"

Planet::Planet(double radius) : Drawable(), _radius(radius), _factor(1.0 / (1.025 * _radius)) {
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

	double water_level = _radius;
	glm::dmat4 water_scale = glm::dmat4(glm::scale(glm::dvec3(water_level)));
	_north_water = std::make_shared<Water>(water_level, north_trans, north_rot, water_scale);
	_south_water = std::make_shared<Water>(water_level, south_trans, south_rot, water_scale);
	_west_water = std::make_shared<Water>(water_level, west_trans, west_rot, water_scale);
	_east_water = std::make_shared<Water>(water_level, east_trans, east_rot, water_scale);
	_hither_water = std::make_shared<Water>(water_level, hither_trans, hither_rot, water_scale);
	_yon_water = std::make_shared<Water>(water_level, yon_trans, yon_rot, water_scale);

	_north = std::make_shared<QuadTree>(north_rot, north_trans, _radius, _radius, _ground_shader);
	_south = std::make_shared<QuadTree>(south_rot, south_trans, _radius, _radius, _ground_shader);
	_west = std::make_shared<QuadTree>(west_rot, west_trans, _radius, _radius, _ground_shader);
	_east = std::make_shared<QuadTree>(east_rot, east_trans, _radius, _radius, _ground_shader);
	_hither = std::make_shared<QuadTree>(hither_rot, hither_trans, _radius, _radius, _ground_shader);
	_yon = std::make_shared<QuadTree>(yon_rot, yon_trans, _radius, _radius, _ground_shader);

	_atmosphere = std::make_shared<AtmosphereObject>(_radius);
}

void Planet::create_color_ramp_texture() {
	_color_ramp_id = Texture2DLoader::load("Images/color_ramp_terrain.png", false, GL_CLAMP_TO_EDGE, GL_REPEAT, GL_LINEAR, GL_LINEAR);
	_grass_n_id = Texture2DLoader::load("Images/grass_01/normal.tga", true, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	_ground_n_id = Texture2DLoader::load("Images/ground_03/normal.tga", true, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	_rock_n_id = Texture2DLoader::load("Images/stone_00/normal.tga", true, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
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

	//Some predraw stuff
	_ground_shader->use();
	setup_terrain_textures();
	setup_atmosphere(camera);

	//Draw stuff
	_north->draw(camera, delta_time);
	_south->draw(camera, delta_time);
	_west->draw(camera, delta_time);
	_east->draw(camera, delta_time);
	_hither->draw(camera, delta_time);
	_yon->draw(camera, delta_time);

	//_north_water->draw(camera, delta_time);
	//_south_water->draw(camera, delta_time);
	//_west_water->draw(camera, delta_time);
	//_east_water->draw(camera, delta_time);
	//_hither_water->draw(camera, delta_time);
	//_yon_water->draw(camera, delta_time);
	
	_atmosphere->draw(camera, delta_time);
}

void Planet::setup_terrain_textures() {
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
	//Terrain texturing
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _grass_n_id);
	glUniform1i(glGetUniformLocation(_ground_shader->program, "grassNormalTex"), 3);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, _ground_n_id);
	glUniform1i(glGetUniformLocation(_ground_shader->program, "groundNormalTex"), 4);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, _rock_n_id);
	glUniform1i(glGetUniformLocation(_ground_shader->program, "rockNormalTex"), 5);
}

void Planet::setup_atmosphere(const Camera &camera) {

	glm::dvec3 cam_pos = camera.get_deye() * _factor;
	double h = length(cam_pos);
	double h2 = h * h;

	glUniform3fv(glGetUniformLocation(_ground_shader->program, "cameraPos"), 1, glm::value_ptr(glm::vec3(cam_pos)));
	glUniform1f(glGetUniformLocation(_ground_shader->program, "cameraHeight"), h);
	glUniform1f(glGetUniformLocation(_ground_shader->program, "cameraHeight2"), h2);

	double ir_scaled = _radius * _factor;
	double or_scaled = 1.0;
	double or_scaled2 = 1.0;

	double scale = 1.0 / (or_scaled - ir_scaled);
	double scale_depth = 0.25;
	double scale_over_depth = scale / scale_depth;

	const double kr = 0.0025;
	const double km = 0.001;
	const double e_sun = 20.0;
	const double f_pi = 4.0 * glm::pi<double>();
	const double g = -0.99;

	glm::vec3 inv_wave_length = glm::vec3(0.65, 0.57, 0.475);
	inv_wave_length = 1.0f / glm::pow(inv_wave_length, glm::vec3(4.0f));
	glUniform1f(glGetUniformLocation(_ground_shader->program, "innerRadius"), ir_scaled);
	glUniform1f(glGetUniformLocation(_ground_shader->program, "outerRadius"), or_scaled);
	glUniform1f(glGetUniformLocation(_ground_shader->program, "outerRadius2"), or_scaled2);
	glUniform1f(glGetUniformLocation(_ground_shader->program, "scale"), scale);
	glUniform1f(glGetUniformLocation(_ground_shader->program, "scaleDepth"), scale_depth);
	glUniform1f(glGetUniformLocation(_ground_shader->program, "scaleOverDepth"), scale_over_depth);
	glUniform1f(glGetUniformLocation(_ground_shader->program, "Kr4Pi"), kr * f_pi);
	glUniform1f(glGetUniformLocation(_ground_shader->program, "Km4Pi"), km * f_pi);
	glUniform1f(glGetUniformLocation(_ground_shader->program, "KmEsun"), km * e_sun);
	glUniform1f(glGetUniformLocation(_ground_shader->program, "KrEsun"), kr * e_sun);
	glUniform1f(glGetUniformLocation(_ground_shader->program, "g"), g);
	glUniform1f(glGetUniformLocation(_ground_shader->program, "g2"), g * g);
	glUniform3fv(glGetUniformLocation(_ground_shader->program, "invWaveLength"), 1, glm::value_ptr(inv_wave_length));
	glUniform3fv(glGetUniformLocation(_ground_shader->program, "lightDir"), 1, glm::value_ptr(DayNight::instance().get_sun()));
}

void Planet::draw_wireframe(const Camera & camera, double delta_time) {
	draw(camera, delta_time);
}
