#pragma once
#include <queue>
#include <mutex>
#include <memory>
#include "Drawable.h"
class Renderer {
public:
	//Singleton
	static Renderer &instance() {
		static Renderer instance;
		return instance;
	}
	void render(double delta_time);	
	void add_drawable(const std::shared_ptr<Drawable> &drawable);
private:
	//Private constructor for singleton
	Renderer() = default;
	std::mutex _render_queue_mutex;
	std::queue<std::shared_ptr<Drawable>> _render_queue;
};