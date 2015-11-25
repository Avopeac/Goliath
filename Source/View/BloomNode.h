#pragma once
#include <memory>
#include "PostProcessingNode.h"
#include "RenderTexture.h"
#include "ScreenQuad.h"
#include "Input/Input.h"
#include "ShaderStore.h"
class BloomNode : public PostProcessingNode {
public:
	BloomNode(int blur_passes, float blur_size, float src_scale, float dst_scale)
		: PostProcessingNode(), _blur_passes(blur_passes), _blur_size(blur_size), _src_scale(src_scale), _dst_scale(dst_scale) {
		//DEBUG
		TwAddSeparator(Input::_tw_bar, "Bloom Effect", NULL);
		TwAddVarRW(Input::_tw_bar, "Blur Passes", TW_TYPE_INT32, &_blur_passes, " min=0 max=5 step=1 ");
		TwAddVarRW(Input::_tw_bar, "Blur Size", TW_TYPE_FLOAT, &_blur_size, " min=0 max=5 step=0.01 ");
		TwAddVarRW(Input::_tw_bar, "Source Factor", TW_TYPE_FLOAT, &_src_scale, " min=0 max=2.0 step=0.1 ");
		TwAddVarRW(Input::_tw_bar, "Destination Factor", TW_TYPE_FLOAT, &_dst_scale, " min=0 max=2.0 step=0.1 ");
	}

	~BloomNode() override {
		_temp.release();
	}

	void apply(const ScreenQuad &quad, const Camera &camera, RenderTexture &src, RenderTexture &dst) override {
		_threshold_shader->use();
		RenderTexture::use(&_temp, &src, nullptr);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1i(glGetUniformLocation(_threshold_shader->program, "texUnit"), 0);
		quad.draw();

		for (int i = 0; i < _blur_passes; ++i) {
			_blur_filter_shader->use();
			RenderTexture::use(&dst, &_temp, nullptr);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform1f(glGetUniformLocation(_blur_filter_shader->program, "size"), _blur_size);
			glUniform2f(glGetUniformLocation(_blur_filter_shader->program, "direction"), 1, 0);
			glUniform1i(glGetUniformLocation(_blur_filter_shader->program, "texUnit"), 0);
			quad.draw();
			RenderTexture::use(&_temp, &dst, nullptr);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform2f(glGetUniformLocation(_blur_filter_shader->program, "direction"), 0, 1);
			glUniform1i(glGetUniformLocation(_blur_filter_shader->program, "texUnit"), 0);
			quad.draw();
		}
		
		RenderTexture::use(&dst, &src, &_temp);
		_additive_shader->use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1f(glGetUniformLocation(_additive_shader->program, "texScale1"), _src_scale);
		glUniform1f(glGetUniformLocation(_additive_shader->program, "texScale2"), _dst_scale);
		glUniform1i(glGetUniformLocation(_additive_shader->program, "texUnit1"), 0);
		glUniform1i(glGetUniformLocation(_additive_shader->program, "texUnit2"), 1);
		quad.draw();
	}
private:
	RenderTexture _temp;
	float _src_scale, _dst_scale, _blur_size;
	int _blur_passes;
	std::shared_ptr<Shader> _additive_shader = ShaderStore::instance().get_shader_from_store(ADDITIVE_SHADER_PATH);
	std::shared_ptr<Shader> _threshold_shader = ShaderStore::instance().get_shader_from_store(THRESHOLD_SHADER_PATH);
	std::shared_ptr<Shader> _blur_filter_shader = ShaderStore::instance().get_shader_from_store(BLUR_SHADER_PATH);
};
