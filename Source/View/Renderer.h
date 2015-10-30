#pragma once
#include <queue>
#include <mutex>
#include <memory>
#include "Drawable.h"
#include "Camera.h"
#include "..\Model\Shader.h"
#include "..\Model\Mesh.h"
#include "..\Model\Lighting.h"
class Renderer {
public:
	//Singleton
	static Renderer &instance() {
		static Renderer instance;
		return instance;
	}

	void render(const Camera &camera, double delta_time);
	void initialize();
	void add_drawable(const std::shared_ptr<Drawable> &drawable);
	const std::shared_ptr<Shader> &get_standard_shader() const { return standard; }

private:

	//Private constructor for singleton
	Renderer() = default;
	std::mutex _render_queue_mutex;
	std::queue<std::shared_ptr<Drawable>> _render_queue;

	//Standard rendering
	const unsigned int num_lights = 3;
	Lighting lighting;
	std::shared_ptr<Shader> standard;
};