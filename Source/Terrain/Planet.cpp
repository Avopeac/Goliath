#include "..\Terrain\Planet.h"
#include "..\View\Renderer.h"

void Planet::draw(const Camera & camera, double delta_time)
{
	_ground_shader->use();

	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, _noise1.texid);
	//glUniform1i(glGetUniformLocation(_ground_shader->program, "noiseCube"), 2);
	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, _normal_noise1.texid);
	//glUniform1i(glGetUniformLocation(_ground_shader->program, "normalNoiseCube"), 3);

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
