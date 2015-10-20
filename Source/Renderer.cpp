#include "Renderer.h"

void Renderer::add_drawable(const std::shared_ptr<Drawable> &drawable) {
	_render_queue_mutex.lock();
	_render_queue.push(drawable);
	_render_queue_mutex.unlock();
}

void Renderer::render(double delta_time) {
	_render_queue_mutex.lock();
	const std::shared_ptr<Drawable> drawable = _render_queue.front();
	_render_queue.pop();
	_render_queue_mutex.unlock();
	drawable->draw(delta_time);
}
