#pragma once
#include <queue>
#include <mutex>
#include <memory>
#include "Drawable.h"
#include "Camera.h"
#include "RenderTexture.h"
#include "..\Model\Shader.h"
#include "..\Model\Mesh.h"
#include "..\Model\Lighting.h"
#include "PostProcessing.h"
#include "PostProcessingNode.h"
#include "ScreenQuad.h"
class Renderer {
public:
	//Singleton
	static Renderer &instance() {
		static Renderer instance;
		return instance;
	}
	void initialize();
	void render(const Camera &camera, double delta_time);
	void add_drawable(const std::shared_ptr<Drawable> &drawable);
	const std::shared_ptr<Shader> &get_standard_shader() const { return _standard_shader; }

private:

	//Private constructor for singleton
	Renderer() {};
	void draw_queue(const Camera &camera, double delta_time);
	std::mutex _render_queue_mutex;
	std::queue<std::shared_ptr<Drawable>> _render_queue;

	//Standard rendering
	const unsigned int _num_lights = 3;
	Lighting _lighting;
	std::shared_ptr<Shader> _standard_shader = std::make_shared<Shader>(STANDARD_VERT_SHADER, STANDARD_FRAG_SHADER);
	void set_standard_uniform(const Camera &camera);
	PostProcessing _post_processing;
};