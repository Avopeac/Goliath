#pragma once
#include "RenderTexture.h"
class PostProcessingNode {
public:
	virtual ~PostProcessingNode() {}
	virtual void apply(const ScreenQuad &quad, RenderTexture &src, RenderTexture &dst) = 0;
protected:
	PostProcessingNode() {}
};