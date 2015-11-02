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
	Renderer() = default;
	void draw_queue(const Camera &camera, double delta_time);
	std::mutex _render_queue_mutex;
	std::queue<std::shared_ptr<Drawable>> _render_queue;

	//Standard rendering
	const unsigned int _num_lights = 3;
	Lighting _lighting;
	std::shared_ptr<Shader> _standard_shader;
	void set_standard_uniform(const Camera &camera);

	//For post processing
	RenderTexture _camera_target_texture;
	RenderTexture _rt2;
	Shader _plain_texture_shader = Shader("Shaders/plaintextureshader.vert", "Shaders/plaintextureshader.frag");
	ScreenQuad _quad;
	PostProcessing _post_processing;
};