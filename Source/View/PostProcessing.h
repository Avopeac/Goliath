#pragma once
#include <vector>
#include "ScreenQuad.h"
#include "RenderTexture.h"
#include "PostProcessingNode.h"
class PostProcessing {
public:
	PostProcessing () {
		temp_render_texture.initialize();
	}

	~PostProcessing() {
		temp_render_texture.release();
	}

	void add_node(std::shared_ptr<PostProcessingNode> node){
		_nodes.push_back(node);
	}

	void apply(const ScreenQuad &quad, RenderTexture &src, RenderTexture &dst) {
		size_t size = _nodes.size();
		//Apply post processing in ping-ponging fashion
		for (unsigned int i = 0; i < size; ++i) {
			if (i % 2 == 0) {
				_nodes[i]->apply(quad, src, temp_render_texture);
			}
			else {
				_nodes[i]->apply(quad, temp_render_texture, src);
			}
		}
		//Our applied post processing will be in either one of these
		size % 2 == 0 ? dst = temp_render_texture : dst = src;
	}

private:
	std::vector<std::shared_ptr<PostProcessingNode>> _nodes;
	RenderTexture temp_render_texture;
};