#pragma once
#include "RenderTexture.h"
#include "Camera.h"
class PostProcessingNode {
public:
	virtual ~PostProcessingNode() {}
	virtual void apply(const ScreenQuad &quad, const Camera &camera, RenderTexture &src, RenderTexture &dst) = 0;
protected:
	PostProcessingNode() {}
};