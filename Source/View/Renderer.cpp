#include "Renderer.h"
#include <vector>
#include <GLM\gtc\type_ptr.hpp>
#include <GLFW\glfw3.h>
#include <AntTweakBar\AntTweakBar.h>
void Renderer::add_drawable(const std::shared_ptr<Drawable> &drawable) {
	_render_queue_mutex.lock();
	_render_queue.push(drawable);
	_render_queue_mutex.unlock();
}

void Renderer::initialize() {
	std::vector<glm::vec3> directions;
	directions.push_back({ 1,1,1 }); //sun
	directions.push_back({ 0,1,0 }); //sky
	directions.push_back({ -1,0,-1 }); //indirect sun

	std::vector<glm::vec3> intensities;
	intensities.push_back({ 1.5, 1.5, 1.0 }); //sun color
	intensities.push_back({ 0.11, 0.161, 0.184 }); //sky color
	intensities.push_back({ 0.15, 0.15, 0.1 }); //indirect sun color

	//Set up default lighting
	lighting = Lighting(3, directions, intensities);
	standard = std::make_shared<Shader>(STANDARD_VERT_SHADER, STANDARD_FRAG_SHADER);
}

void Renderer::render(const Camera &camera, double delta_time) {

	standard->use();
	glUniformMatrix4fv(glGetUniformLocation(standard->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
	glUniformMatrix4fv(glGetUniformLocation(standard->program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	
	if (lighting.get_num_lights() > 0) {
		glUniform1i(glGetUniformLocation(standard->program, "lights"), lighting.get_num_lights());
		glUniform3fv(glGetUniformLocation(standard->program, "directions"), lighting.get_num_lights(), &lighting.directions[0][0]);
		glUniform3fv(glGetUniformLocation(standard->program, "intensities"), lighting.get_num_lights(), &lighting.intensities[0][0]);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_render_queue_mutex.lock();
	std::shared_ptr<Drawable> drawable;
	while (!_render_queue.empty()) {
		drawable = _render_queue.front();
		_render_queue.pop();
		if (drawable != nullptr) {
  			drawable->draw(lighting, camera, delta_time);
		}
	}
	_render_queue_mutex.unlock();

	//Draw AntTweakBar
	TwDraw();
}
