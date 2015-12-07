#pragma once
#include <memory>
#include "PostProcessingNode.h"
#include "ScreenQuad.h"
#include "Model/Shader.h"
#include "Input/Input.h"
#include "ShaderStore.h"
class FxaaNode : public PostProcessingNode {
public:
	FxaaNode() : PostProcessingNode() {}
	void apply(const ScreenQuad &quad, const Camera &camera, RenderTexture &src, RenderTexture &dst) override {
		_fxaa_shader->use();
		RenderTexture::use(&dst, &src, nullptr);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1i(glGetUniformLocation(_fxaa_shader->program, "texUnit"), 0);
		quad.draw();
	}
private:
	std::shared_ptr<Shader> _fxaa_shader = ShaderStore::instance().get_shader_from_store(FXAA_SHADER_PATH);
};