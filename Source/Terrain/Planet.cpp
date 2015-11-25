#include "Planet.h"
#include "View/Renderer.h"
#include "View/ShaderStore.h"
#include "Model/CubeMap.h"
#include "Terrain/QuadTree.h"
#include "Terrain/Noise3D.h"
#include <GLM/gtx/transform.hpp>
#include "Terrain/Water.h"
#include <SOIL/SOIL.h>

Planet::Planet(double radius) : Drawable(), _radius(radius) {
	setup_cube();
	setup_skybox();
	create_color_ramp_texture();
	noise_maker.initialize();
}

void Planet::setup_cube() {
	double scale = _radius;
	double trans = _radius * 0.5;
	_ground_shader = ShaderStore::instance().get_shader_from_store(GROUND_SHADER_PATH);
	_north = std::make_shared<QuadTree>(glm::dmat4(1), glm::translate(glm::dvec3(0, trans, 0)), scale, _radius, _ground_shader);
	_south = std::make_shared<QuadTree>(glm::rotate(glm::pi<double>(), glm::dvec3(0, 0, 1)), glm::translate(glm::dvec3(0, -trans, 0)), scale, _radius, _ground_shader);
	_west = std::make_shared<QuadTree>(glm::rotate(glm::half_pi<double>(), glm::dvec3(0, 0, 1)), glm::translate(glm::dvec3(-trans, 0, 0)), scale, _radius, _ground_shader);
	_east = std::make_shared<QuadTree>(glm::rotate(glm::three_over_two_pi<double>(), glm::dvec3(0, 0, 1)), glm::translate(glm::dvec3(trans, 0, 0)), scale, _radius, _ground_shader);
	_hither = std::make_shared<QuadTree>(glm::rotate(glm::half_pi<double>(), glm::dvec3(1, 0, 0)), glm::translate(glm::dvec3(0, 0, trans)), scale, _radius, _ground_shader);
	_yon = std::make_shared<QuadTree>(glm::rotate(glm::three_over_two_pi<double>(), glm::dvec3(1, 0, 0)), glm::translate(glm::dvec3(0, 0, -trans)), scale, _radius, _ground_shader);

	double water_level = _radius;
	_north_water = std::make_shared<Water>(water_level, glm::translate(glm::vec3(0, trans, 0)), glm::scale(glm::vec3(scale)), glm::mat4(1));
	_south_water = std::make_shared<Water>(water_level, glm::translate(glm::vec3(0, -trans, 0)), glm::scale(glm::vec3(scale)), glm::rotate(glm::pi<float>(), glm::vec3(0, 0, 1)));
	_west_water = std::make_shared<Water>(water_level, glm::translate(glm::vec3(-trans, 0, 0)), glm::scale(glm::vec3(scale)), glm::rotate(glm::half_pi<float>(), glm::vec3(0, 0, 1)));
	_east_water = std::make_shared<Water>(water_level, glm::translate(glm::vec3(trans, 0, 0)), glm::scale(glm::vec3(scale)), glm::rotate(glm::three_over_two_pi<float>(), glm::vec3(0, 0, 1)));
	_hither_water = std::make_shared<Water>(water_level, glm::translate(glm::vec3(0, 0, trans)), glm::scale(glm::vec3(scale)), glm::rotate(glm::half_pi<float>(), glm::vec3(1, 0, 0)));
	_yon_water = std::make_shared<Water>(water_level, glm::translate(glm::vec3(0, 0, -trans)), glm::scale(glm::vec3(scale)), glm::rotate(glm::three_over_two_pi<float>(), glm::vec3(1, 0, 0)));
}

void Planet::create_color_ramp_texture() {
	glGenTextures(1, &_color_ramp_id);
	glBindTexture(GL_TEXTURE_2D, _color_ramp_id);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	int w, h, c;
	unsigned char *data = SOIL_load_image("Images/color_ramp_terrain.png", &w, &h, &c, SOIL_LOAD_RGB);
	if (data != 0) {
		std::cout << "Loaded color ramp texture. " << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		SOIL_free_image_data(data);
	}
	else {
		std::cout << "Failed to load color ramp texture. " << std::endl;
	}
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
	
	glUniformMatrix4fv(glGetUniformLocation(_ground_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(camera.get_dview())));
	glUniformMatrix4fv(glGetUniformLocation(_ground_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(glm::mat4(camera.get_dprojection())));

	_skybox->draw(camera, delta_time);

	_north->draw(camera, delta_time);
	_south->draw(camera, delta_time);
	_west->draw(camera, delta_time);
	_east->draw(camera, delta_time);
	_hither->draw(camera, delta_time);
	_yon->draw(camera, delta_time);

	_north_water->draw(camera, delta_time);
	_south_water->draw(camera, delta_time);
	_west_water->draw(camera, delta_time);
	_east_water->draw(camera, delta_time);
	_hither_water->draw(camera, delta_time);
	_yon_water->draw(camera, delta_time);
}

void Planet::draw_wireframe(const Camera & camera, double delta_time)
{
	_ground_shader->use();

	glUniformMatrix4fv(glGetUniformLocation(_ground_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(camera.get_dview())));
	glUniformMatrix4fv(glGetUniformLocation(_ground_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(glm::mat4(camera.get_dprojection())));

	_skybox->draw(camera, delta_time);

	_north->draw(camera, delta_time);
	_south->draw(camera, delta_time);
	_west->draw(camera, delta_time);
	_east->draw(camera, delta_time);
	_hither->draw(camera, delta_time);
	_yon->draw(camera, delta_time);

	_north_water->draw(camera, delta_time);
	_south_water->draw(camera, delta_time);
	_west_water->draw(camera, delta_time);
	_east_water->draw(camera, delta_time);
	_hither_water->draw(camera, delta_time);
	_yon_water->draw(camera, delta_time);
}
