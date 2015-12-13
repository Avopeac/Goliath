#pragma once
#include <queue>
#include <mutex>
#include <memory>
#include "Drawable.h"
#include "Camera.h"
#include "RenderTexture.h"
#include "Model/Shader.h"
#include "Model/Mesh.h"
#include "Model/CubeMap.h"
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

private:
	//Private constructor for singleton
	Renderer() {};
	void draw_queue(const Camera &camera, double delta_time);
	std::mutex _render_queue_mutex;
	std::queue<std::shared_ptr<Drawable>> _render_queue;
	PostProcessing _post_processing;
	bool wireframe;
};
