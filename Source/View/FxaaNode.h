#pragma once
#include <memory>
#include "PostProcessingNode.h"
#include "ScreenQuad.h"
#include "Model/Shader.h"
#include "Input/Input.h"
#include "ShaderStore.h"
class FxaaNode : public PostProcessingNode {
public:
	FxaaNode() : PostProcessingNode() {
		TwAddSeparator(Input::_tw_bar, "FXAA", NULL);
		TwAddVarRW(Input::_tw_bar, "Subpixel Shift", TW_TYPE_FLOAT, &_subpixel_shift, " min=0.0 max=1.0 step=0.01 ");
		TwAddVarRW(Input::_tw_bar, "Span Max", TW_TYPE_FLOAT, &_span_max, " min=1.0 max=32.0 step=0.2 ");
		TwAddVarRW(Input::_tw_bar, "Reduce Mult", TW_TYPE_FLOAT, &_reduce_mult, " min=1.0 max=256.0 step=0.2 ");
		TwAddVarRW(Input::_tw_bar, "Reduce Min", TW_TYPE_FLOAT, &_reduce_min, " min=1.0 max=256.0 step=0.2 ");
	}
	void apply(const ScreenQuad &quad, const Camera &camera, RenderTexture &src, RenderTexture &dst) override {
		_fxaa_shader->use();
		RenderTexture::use(&dst, &src, nullptr);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1i(glGetUniformLocation(_fxaa_shader->program, "texUnit"), 0);
		glUniform1f(glGetUniformLocation(_fxaa_shader->program, "time"), Application::elapsed_time);
		glUniform1f(glGetUniformLocation(_fxaa_shader->program, "subpixelShift"), glm::clamp(_subpixel_shift, 0.0f, 1.0f));
		glUniform1f(glGetUniformLocation(_fxaa_shader->program, "spanMax"), _span_max);
		glUniform1f(glGetUniformLocation(_fxaa_shader->program, "reduceMult"), 1.0f / glm::clamp(_reduce_mult, 1.0f, 256.0f));
		glUniform1f(glGetUniformLocation(_fxaa_shader->program, "reduceMin"), 1.0f / glm::clamp(_reduce_min, 1.0f, 256.0f));
		quad.draw();
	}
private:
	std::shared_ptr<Shader> _fxaa_shader = ShaderStore::instance().get_shader_from_store(FXAA_SHADER_PATH);
	float _subpixel_shift = 0.25f;
	float _span_max = 8.0f;
	float _reduce_mult = 8.0f;
	float _reduce_min = 128.0f;
};