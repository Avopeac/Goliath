#include "Planet.h"
#include "View/Renderer.h"
#include "View/ShaderStore.h"
#include "Model/CubeMap.h"
#include "Terrain/QuadTree.h"
#include <GLM\gtc\random.hpp>
#include <GLM\gtx\transform.hpp>
#include <SOIL/SOIL.h>
#include <iterator>
#include <algorithm>
#include <set>

Planet::Planet(double radius) : Drawable(), _radius(radius) {
	setup_cube();
	setup_skybox();
	create_color_ramp_texture();
	create_gradient_array();
	create_permutation_array();
	create_permutation_texture();
}


void Planet::setup_cube() {
	float scale = (float)_radius;
	float trans = (float)_radius * 0.5f;
	_ground_shader = ShaderStore::instance().get_shader_from_store(GROUND_SHADER_PATH);
	_north = std::make_shared<QuadTree>(glm::mat4(1), glm::translate(glm::vec3(0, trans, 0)), scale, _ground_shader);
	_south = std::make_shared<QuadTree>(glm::rotate(glm::pi<float>(), glm::vec3(0, 0, 1)), glm::translate(glm::vec3(0, -trans, 0)), scale, _ground_shader);
	_west = std::make_shared<QuadTree>(glm::rotate(glm::half_pi<float>(), glm::vec3(0, 0, 1)), glm::translate(glm::vec3(-trans, 0, 0)), scale, _ground_shader);
	_east = std::make_shared<QuadTree>(glm::rotate(glm::three_over_two_pi<float>(), glm::vec3(0, 0, 1)), glm::translate(glm::vec3(trans, 0, 0)), scale, _ground_shader);
	_hither = std::make_shared<QuadTree>(glm::rotate(glm::half_pi<float>(), glm::vec3(1, 0, 0)), glm::translate(glm::vec3(0, 0, trans)), scale, _ground_shader);
	_yon = std::make_shared<QuadTree>(glm::rotate(glm::three_over_two_pi<float>(), glm::vec3(1, 0, 0)), glm::translate(glm::vec3(0, 0, -trans)), scale, _ground_shader);
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

void Planet::create_permutation_array() {
	permutations = { 151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
}

void Planet::create_gradient_array() {
	gradients = { {1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0},
	{1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},
	{0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1}
	};
}

void Planet::create_permutation_texture() {
	glGenTextures(1, &_permutation_id);
	glBindTexture(GL_TEXTURE_1D, _permutation_id);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, &permutations[0]);
}

void Planet::setup_skybox() {
	//Set up skybox shader
	_skybox = std::make_shared<Skybox>();
	_skybox->set_shader(ShaderStore::instance().get_shader_from_store(SKYBOX_SHADER_PATH));
}

void Planet::draw(const Camera & camera, double delta_time)
{
	_ground_shader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _color_ramp_id);
	glUniform1i(glGetUniformLocation(_ground_shader->program, "colorRampTex"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, _permutation_id);
	glUniform1i(glGetUniformLocation(_ground_shader->program, "permutationTex"), 1);
	glUniform3fv(glGetUniformLocation(_ground_shader->program, "gradients"), 12, &gradients[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(_ground_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	glUniformMatrix4fv(glGetUniformLocation(_ground_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
	_skybox->draw(camera, delta_time);
	_north->draw(camera, delta_time);
	_south->draw(camera, delta_time);
	_west->draw(camera, delta_time);
	_east->draw(camera, delta_time);
	_hither->draw(camera, delta_time);
	_yon->draw(camera, delta_time);
}

void Planet::draw_wireframe(const Camera & camera, double delta_time)
{
	_ground_shader->use();
	glUniformMatrix4fv(glGetUniformLocation(_ground_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	glUniformMatrix4fv(glGetUniformLocation(_ground_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
	_skybox->draw(camera, delta_time);
	_north->draw(camera, delta_time);
	_south->draw(camera, delta_time);
	_west->draw(camera, delta_time);
	_east->draw(camera, delta_time);
	_hither->draw(camera, delta_time);
	_yon->draw(camera, delta_time);
}
