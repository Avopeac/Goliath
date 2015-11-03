#pragma once
#include "PostProcessingNode.h"
#include "ScreenQuad.h"
#include "..\Model\Shader.h"
#include "..\Input\Input.h"
class GammaNode : public PostProcessingNode {
public:
	GammaNode(float gamma_exponent) : PostProcessingNode(), _gamma_exponent(gamma_exponent) {
		//Debug
		TwAddSeparator(Input::_tw_bar, "Gamma Effect", NULL);
		TwAddVarRW(Input::_tw_bar, "Gamma", TW_TYPE_FLOAT, &_gamma_exponent, " min=0.0 max=4.0 step=0.1");
	}
	void apply(const ScreenQuad &quad, const Camera &camera, RenderTexture &src, RenderTexture &dst) override {
		_gamma_shader.use();
		RenderTexture::use(&dst, &src, nullptr);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1f(glGetUniformLocation(_gamma_shader.program, "gamma"), _gamma_exponent);
		glUniform1i(glGetUniformLocation(_gamma_shader.program, "texUnit"), 0);
		quad.draw();
	}
private:
	float _gamma_exponent;
	Shader _gamma_shader = Shader("Shaders/gammacorrectionshader.vert","Shaders/gammacorrectionshader.frag");
};