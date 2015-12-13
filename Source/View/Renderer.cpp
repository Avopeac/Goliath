#include "Renderer.h"
#include <GLM/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <AntTweakBar/AntTweakBar.h>
#include "FxaaNode.h"
#include "BloomNode.h"
#include "GammaToneMapNode.h"

void Renderer::add_drawable(const std::shared_ptr<Drawable> &drawable) {
	_render_queue.push(drawable);
}

void Renderer::initialize() {
	wireframe = false;
	TwAddSeparator(Input::_tw_bar, "Renderer", nullptr);
	TwAddVarRW(Input::_tw_bar, "Wireframe", TW_TYPE_BOOLCPP, &wireframe, "");
	//Set up post processing
	_post_processing.add_node(std::make_shared<BloomNode>(2, 0.42f, 1.0f, 1.0f));
	_post_processing.add_node(std::make_shared<GammaToneMapNode>(1.6f, 2.2f));
	_post_processing.add_node(std::make_shared<FxaaNode>());
}

void Renderer::render(const Camera &camera, double delta_time) {
	_post_processing.capture();
	draw_queue(camera, delta_time);
	_post_processing.apply(camera);
	//Draw AntTweakBar
	TwDraw();
}

void Renderer::draw_queue(const Camera &camera, double delta_time) {
	while (!_render_queue.empty()) {
		if (wireframe) {
			_render_queue.front()->draw_wireframe(camera, delta_time);
		}
		else {
			_render_queue.front()->draw(camera, delta_time);
		}
		_render_queue.pop();
	}
}
