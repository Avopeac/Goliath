#include "Renderer.h"
#include <vector>
#include <GLM\gtc\type_ptr.hpp>
#include <GLFW\glfw3.h>
#include <AntTweakBar\AntTweakBar.h>
#include "BloomNode.h"
#include "GammaNode.h"
#include "ToneMappingNode.h"

void Renderer::add_drawable(const std::shared_ptr<Drawable> &drawable) {
	_render_queue_mutex.lock();
	_render_queue.push(drawable);
	_render_queue_mutex.unlock();
}

void Renderer::initialize() {
	//Set up default lighting
	std::vector<glm::vec3> directions;
	directions.push_back({ 1,1,1 }); //sun
	directions.push_back({ 0,1,0 }); //sky
	directions.push_back({ -1,0,-1 }); //indirect sun
	std::vector<glm::vec3> intensities;
	intensities.push_back({ 1.5, 1.5, 1.0 }); //sun color
	intensities.push_back({ 0.11, 0.161, 0.184 }); //sky color
	intensities.push_back({ 0.15, 0.15, 0.1 }); //indirect sun color
	_lighting = Lighting(3, directions, intensities);
	//Set up post processing
	_camera_target_texture.initialize();
	_post_processing.add_node(std::make_shared<BloomNode>(5, 5, 1.0f, 1.0f));
	_post_processing.add_node(std::make_shared<GammaNode>(2.2f));
	_post_processing.add_node(std::make_shared<ToneMappingNode>(1.0f));
}

void Renderer::render(const Camera &camera, double delta_time) {
	set_standard_uniform(camera);
	//Render to target texture
	RenderTexture::use(&_camera_target_texture, nullptr, nullptr);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	draw_queue(camera, delta_time);
	//Apply post processing
	_post_processing.apply(_quad, _camera_target_texture, _post_processing_target_texture);
	RenderTexture::use(nullptr, &_post_processing_target_texture, nullptr);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_quad.draw();
	//Draw AntTweakBar
	TwDraw();
}

void Renderer::draw_queue(const Camera &camera, double delta_time) {
	_render_queue_mutex.lock();
	std::shared_ptr<Drawable> drawable;
	while (!_render_queue.empty()) {
		drawable = _render_queue.front();
		_render_queue.pop();
		if (drawable != nullptr) {
			drawable->draw(_lighting, camera, delta_time);
		}
	}
	_render_queue_mutex.unlock();
}

void Renderer::set_standard_uniform(const Camera &camera) {
	//Set up standard shader lighting
	_standard_shader->use();
	glUniformMatrix4fv(glGetUniformLocation(_standard_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
	glUniformMatrix4fv(glGetUniformLocation(_standard_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	if (_lighting.get_num_lights() > 0) {
		for (int i = 0; i < _lighting.get_num_lights(); ++i) {
			_lighting.view_space_directions[i] = glm::mat3(camera.get_view()) * _lighting.directions[i];
		}
		glUniform1i(glGetUniformLocation(_standard_shader->program, "lights"), _lighting.get_num_lights());
		glUniform3fv(glGetUniformLocation(_standard_shader->program, "directions"), _lighting.get_num_lights(), &_lighting.view_space_directions[0][0]);
		glUniform3fv(glGetUniformLocation(_standard_shader->program, "intensities"), _lighting.get_num_lights(), &_lighting.intensities[0][0]);
	}
}
