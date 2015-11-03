#pragma once
#include <vector>
#include "ScreenQuad.h"
#include "RenderTexture.h"
#include "PostProcessingNode.h"
class PostProcessing {
public:
	PostProcessing() {}
	~PostProcessing() {
		temp_render_texture1.release();
		temp_render_texture2.release();
	}

	void add_node(std::shared_ptr<PostProcessingNode> node) {
		_nodes.push_back(node);
	}

	void apply(const ScreenQuad &quad, const Camera &camera, RenderTexture &src, RenderTexture &dst) {
		size_t size = _nodes.size();
		//Move src into first render texture
		if (size == 1) {
			_nodes.front()->apply(quad, camera, src, dst);
		}
		else if (size > 1) {
			_nodes.front()->apply(quad, camera, src, temp_render_texture1);
			//Apply post processing in ping-ponging fashion
			RenderTexture first = temp_render_texture1;
			RenderTexture second = temp_render_texture2;
			for (unsigned int i = 1; i < size; ++i) {
				_nodes[i]->apply(quad, camera, first, second);
				RenderTexture temp = first;
				first = second;
				second = temp;
			}
			dst = first;
		}
		else if (size == 0) {
			dst = src;
		}
	}

private:
	std::vector<std::shared_ptr<PostProcessingNode>> _nodes;
	RenderTexture temp_render_texture1;
	RenderTexture temp_render_texture2;
};