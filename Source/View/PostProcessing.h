#pragma once
#include <vector>
#include "ScreenQuad.h"
#include "RenderTexture.h"
#include "PostProcessingNode.h"
class PostProcessing {
public:
	PostProcessing () {}

	void add_node(std::shared_ptr<PostProcessingNode> node){
		_nodes.push_back(node);
	}

	void apply(const ScreenQuad &quad, RenderTexture &src, RenderTexture &dst) {
		for (auto &node : _nodes) {
			node->apply(quad, src, dst);
		}
	}

private:
	std::vector<std::shared_ptr<PostProcessingNode>> _nodes;
};