#include "Terrain/Planet.h"
#include "View/Drawable.h"
#include "Terrain/QuadTree.h"
#include "Terrain/Skybox.h"
#include "View/ShaderStore.h"
#include "Model/CubeMap.h"
#include <SOIL/SOIL.h>
#include <GLM/gtx/transform.hpp>

Planet::Planet(double radius) : Drawable() {

	_ground_shader = ShaderStore::instance().get_shader_from_store(GROUND_SHADER_PATH);

	float scale = (float)radius;
	float trans = (float)radius * 0.5f;
	//Set up planet "cube"
	_north = std::make_shared<QuadTree>(glm::mat4(1), glm::translate(glm::vec3(0, trans, 0)), scale, _ground_shader);
	_south = std::make_shared<QuadTree>(glm::rotate(glm::pi<float>(), glm::vec3(0, 0, 1)), glm::translate(glm::vec3(0, -trans, 0)), scale, _ground_shader);
	_west = std::make_shared<QuadTree>(glm::rotate(glm::half_pi<float>(), glm::vec3(0, 0, 1)), glm::translate(glm::vec3(-trans, 0, 0)), scale, _ground_shader);
	_east = std::make_shared<QuadTree>(glm::rotate(glm::three_over_two_pi<float>(), glm::vec3(0, 0, 1)), glm::translate(glm::vec3(trans, 0, 0)), scale, _ground_shader);
	_hither = std::make_shared<QuadTree>(glm::rotate(glm::half_pi<float>(), glm::vec3(1, 0, 0)), glm::translate(glm::vec3(0, 0, trans)), scale, _ground_shader);
	_yon = std::make_shared<QuadTree>(glm::rotate(glm::three_over_two_pi<float>(), glm::vec3(1, 0, 0)), glm::translate(glm::vec3(0, 0, -trans)), scale, _ground_shader);

	//Set up skybox shader
	_skybox = std::make_shared<Skybox>();
	_skybox->set_shader(ShaderStore::instance().get_shader_from_store(SKYBOX_SHADER_PATH));

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
	}
	else {
		std::cout << "Failed to load color ramp texture. " << std::endl;
	}
}

void Planet::draw(const Camera & camera, double delta_time)
{
	_ground_shader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _color_ramp_id);
	glUniform1i(glGetUniformLocation(_ground_shader->program, "colorRampTex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(_ground_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	glUniformMatrix4fv(glGetUniformLocation(_ground_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
	_skybox->draw(camera, delta_time);
	/*_north->draw(camera, delta_time);
	_south->draw(camera, delta_time);
	_west->draw(camera, delta_time);
	_east->draw(camera, delta_time);*/
	_hither->draw(camera, delta_time);
	//_yon->draw(camera, delta_time);

	
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
