#pragma once
#include <memory>
#include "PostProcessingNode.h"
#include "ScreenQuad.h"
#include "Model/Shader.h"
#include "Input/Input.h"
#include "ShaderStore.h"
class GammaToneMapNode : public PostProcessingNode {
public:
	GammaToneMapNode(float exposure_time, float gamma) : PostProcessingNode(), _exposure_time(exposure_time), _gamma(gamma) {
		//Debug
		TwAddSeparator(Input::_tw_bar, "Tone Mapping Effect", NULL);
		TwAddVarRW(Input::_tw_bar, "Exposure", TW_TYPE_FLOAT, &_exposure_time, " min=0.0 max=10.0 step=0.1");
		TwAddVarRW(Input::_tw_bar, "Gamma", TW_TYPE_FLOAT, &_gamma, " min=0.0 max=4.0 step=0.1");
	}
	void apply(const ScreenQuad &quad, const Camera &camera, RenderTexture &src, RenderTexture &dst) override {
		_tone_mapping_shader->use();
		RenderTexture::use(&dst, &src, nullptr);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1f(glGetUniformLocation(_tone_mapping_shader->program, "gamma"), _gamma);
		glUniform1f(glGetUniformLocation(_tone_mapping_shader->program, "exposure"), _exposure_time);
		glUniform1i(glGetUniformLocation(_tone_mapping_shader->program, "texUnit"), 0);
		quad.draw();
	}
private:
	float _gamma;
	float _exposure_time;
	std::shared_ptr<Shader> _tone_mapping_shader = ShaderStore::instance().get_shader_from_store(GAMMA_TONEMAP_SHADER_PATH);
};