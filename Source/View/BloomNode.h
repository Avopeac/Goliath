#pragma once
#include "PostProcessingNode.h"
#include "RenderTexture.h"
#include "ScreenQuad.h"
class BloomNode : public PostProcessingNode {
public:
	BloomNode(int blur_passes, int blur_size, float src_scale, float dst_scale)
		: PostProcessingNode(), _blur_passes(blur_passes), _blur_size(blur_size), _src_scale(src_scale), _dst_scale(dst_scale) {
		_temp.initialize();
	}

	~BloomNode() override {
		_temp.release();
	}

	void apply(const ScreenQuad &quad, RenderTexture &src, RenderTexture &dst) override {
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
		
		//RenderTexture::use(&dst, &_temp)
		_additive_shader.use();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _temp.color);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, src.color);
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
