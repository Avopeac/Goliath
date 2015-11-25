#pragma once
#include <vector>
#include "ScreenQuad.h"
#include "RenderTexture.h"
#include "PostProcessingNode.h"
#include "ShaderStore.h"
class PostProcessing {
public:
	PostProcessing() {}
	~PostProcessing() {
		_temp_render_texture1.release();
		_temp_render_texture2.release();
	}

	void add_node(std::shared_ptr<PostProcessingNode> node) { _nodes.push_back(node); }

	void capture() {
		RenderTexture::use(&_temp_render_texture1, nullptr, nullptr);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	};

	void apply(const Camera &camera) {
		size_t size = _nodes.size();
		if (size == 1) {
			_nodes.front()->apply(_quad, camera, _temp_render_texture1, _temp_render_texture2);
			draw_to_display(_temp_render_texture2);
		}
		else if (size > 1) {
			_nodes.front()->apply(_quad, camera, _temp_render_texture1, _temp_render_texture2);
			//Apply post processing in ping-ponging fashion
			RenderTexture first = _temp_render_texture2;
			RenderTexture second = _temp_render_texture1;
			for (unsigned int i = 1; i < size; ++i) {
				_nodes[i]->apply(_quad, camera, first, second);
				RenderTexture temp = first;
				first = second;
				second = temp;
			}
			draw_to_display(first);
		}
		else if (size == 0) {
			draw_to_display(_temp_render_texture1);
		}
	}

private:
	std::vector<std::shared_ptr<PostProcessingNode>> _nodes;
	ScreenQuad _quad;
	RenderTexture _temp_render_texture1;
	RenderTexture _temp_render_texture2;
	std::shared_ptr<Shader> _plain_texture_shader = ShaderStore::instance().get_shader_from_store(PLAIN_TEXTURE_SHADER_PATH);

	void draw_to_display(RenderTexture &render_texture) {
		//Draw to display
		RenderTexture::use(nullptr, &render_texture, nullptr);
		_plain_texture_shader->use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1i(glGetUniformLocation(_plain_texture_shader->program, "texUnit"), 0);
		glBindTexture(GL_TEXTURE_2D, render_texture.color);
		_quad.draw();
	}
};
