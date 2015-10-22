#include "Renderer.h"
#include <GLFW\glfw3.h>
void Renderer::add_drawable(const std::shared_ptr<Drawable> &drawable) {
	_render_queue_mutex.lock();
	_render_queue.push(drawable);
	_render_queue_mutex.unlock();
}

void Renderer::render(const Camera &camera, double delta_time) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	_render_queue_mutex.lock();
	std::shared_ptr<Drawable> drawable;
	if (!_render_queue.empty()) {
		 drawable = _render_queue.front();
		_render_queue.pop();
	}
	_render_queue_mutex.unlock();

	if (drawable != nullptr) {
		drawable->draw(camera, delta_time);
	}
}
