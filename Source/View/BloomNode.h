#pragma once
#include "PostProcessingNode.h"
#include "RenderTexture.h"
#include "ScreenQuad.h"
#include "..\Input\Input.h"
class BloomNode : public PostProcessingNode {
public:
	BloomNode(int blur_passes, int blur_size, float src_scale, float dst_scale)
		: PostProcessingNode(), _blur_passes(blur_passes), _blur_size(blur_size), _src_scale(src_scale), _dst_scale(dst_scale) {
		//DEBUG
		TwAddSeparator(Input::_tw_bar, "Bloom Effect", NULL);
		TwAddVarRW(Input::_tw_bar, "Blur Passes", TW_TYPE_INT32, &_blur_passes, " min=0 max=25 step=1 ");
		TwAddVarRW(Input::_tw_bar, "Blur Size", TW_TYPE_INT32, &_blur_size, " min=0 max=25 step=1 ");
		TwAddVarRW(Input::_tw_bar, "Source Factor", TW_TYPE_FLOAT, &_src_scale, " min=0 max=2.0 step=0.1 ");
		TwAddVarRW(Input::_tw_bar, "Destination Factor", TW_TYPE_FLOAT, &_dst_scale, " min=0 max=2.0 step=0.1 ");
	}

	~BloomNode() override {
		_temp.release();
	}

	void apply(const ScreenQuad &quad, const Camera &camera, RenderTexture &src, RenderTexture &dst) override {
		_threshold_shader.use();
		RenderTexture::use(&_temp, &src, nullptr);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1i(glGetUniformLocation(_threshold_shader.program, "texUnit"), 0);
		quad.draw();

		for (int i = 0; i < _blur_passes; ++i) {
			_blur_filter_shader.use();
			RenderTexture::use(&dst, &_temp, nullptr);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform1i(glGetUniformLocation(_blur_filter_shader.program, "size"), _blur_size);
			glUniform1i(glGetUniformLocation(_blur_filter_shader.program, "horizontal"), 0);
			glUniform1i(glGetUniformLocation(_blur_filter_shader.program, "texUnit"), 0);
			quad.draw();
			RenderTexture::use(&_temp, &dst, nullptr);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform1i(glGetUniformLocation(_blur_filter_shader.program, "horizontal"), 1);
			glUniform1i(glGetUniformLocation(_blur_filter_shader.program, "texUnit"), 0);
			quad.draw();
		}
		
		RenderTexture::use(&dst, &src, &_temp);
		_additive_shader.use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1f(glGetUniformLocation(_additive_shader.program, "texScale1"), _src_scale);
		glUniform1f(glGetUniformLocation(_additive_shader.program, "texScale2"), _dst_scale);
		glUniform1i(glGetUniformLocation(_additive_shader.program, "texUnit1"), 0);
		glUniform1i(glGetUniformLocation(_additive_shader.program, "texUnit2"), 1);
		quad.draw();
	}
private:
	RenderTexture _temp;
	float _src_scale, _dst_scale;
	int _blur_passes, _blur_size;
	Shader _additive_shader = Shader("Shaders/additiveshader.vert", "Shaders/additiveshader.frag");
	Shader _threshold_shader = Shader("Shaders/thresholdshader.vert", "Shaders/thresholdshader.frag");
	Shader _blur_filter_shader = Shader("Shaders/blurfiltershader.vert", "Shaders/blurfiltershader.frag");
};
