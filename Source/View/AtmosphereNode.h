#pragma once
#include "PostProcessingNode.h"
#include "ScreenQuad.h"
#include "..\Model\Lighting.h"
#include "..\Model\Shader.h"
#include "..\Input\Input.h"
class AtmosphereNode : public PostProcessingNode {
public:
	AtmosphereNode() : PostProcessingNode() {
		TwAddSeparator(Input::_tw_bar, "Atmospheric Scattering", NULL);
		TwAddVarRW(Input::_tw_bar, "Sun Direction X", TW_TYPE_FLOAT, &_sun_direction.x, " min=-1.0 max=1.0 step=0.1 ");
		TwAddVarRW(Input::_tw_bar, "Sun Direction Y", TW_TYPE_FLOAT, &_sun_direction.y, " min=-1.0 max=1.0 step=0.1 ");
		TwAddVarRW(Input::_tw_bar, "Sun Direction Z", TW_TYPE_FLOAT, &_sun_direction.z, " min=-1.0 max=1.0 step=0.1 ");


	}
	void set_planet_world_position(const glm::vec3 &planet_world_pos) { _planet_world_pos = planet_world_pos; }
	void apply(const ScreenQuad &quad, const Camera &camera, RenderTexture &src, RenderTexture &dst) override {
		_atmosphere_shader.use();
		RenderTexture::use(&dst, &src, nullptr);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1i(glGetUniformLocation(_atmosphere_shader.program, "texUnit"), 0);
		//Manually set depth texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, src.depth);
		glUniform1i(glGetUniformLocation(_atmosphere_shader.program, "depthUnit"), 1);
		//Update shader
		update_planets();
		update_solar();
		update_scattering();
		update_transforms(camera);
		//Draw post processing
		quad.draw();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

private:

	void update_scattering() {
		//Upload scattering information
		float scale_radius = 1.0f / (_atmosphere_radius - _planet_radius);
		glUniform1f(glGetUniformLocation(_atmosphere_shader.program, "scaleRadius"), scale_radius);
		glUniform1f(glGetUniformLocation(_atmosphere_shader.program, "scaleHeight"), _scale_height);
		glUniform1f(glGetUniformLocation(_atmosphere_shader.program, "scaleOverHeight"), scale_radius / _scale_height);
		glUniform1f(glGetUniformLocation(_atmosphere_shader.program, "miePhaseG"), _mie_phase_g);
		glUniform1f(glGetUniformLocation(_atmosphere_shader.program, "miePhaseGSqr"), _mie_phase_g * _mie_phase_g);
		glUniform1f(glGetUniformLocation(_atmosphere_shader.program, "mieConstant"), _mie_constant);
		glUniform1f(glGetUniformLocation(_atmosphere_shader.program, "mieConstant4Pi"), 4.0f * glm::pi<float>() * _mie_constant);
		glUniform1f(glGetUniformLocation(_atmosphere_shader.program, "rayleighConstant"), _rayleigh_constant);
		glUniform1f(glGetUniformLocation(_atmosphere_shader.program, "rayleighConstant4Pi"), 4.0f * glm::pi<float>() * _rayleigh_constant);
	}

	void update_transforms(const Camera &camera) {
		//Upload transform information
		glUniform3fv(glGetUniformLocation(_atmosphere_shader.program, "cameraWorldPos"), 1, glm::value_ptr(camera.get_eye()));
		glm::mat4 inverse_view_proj = glm::inverse(camera.get_view()) * glm::inverse(camera.get_perspective());
		glUniformMatrix4fv(glGetUniformLocation(_atmosphere_shader.program, "inverseViewProj"), 1, GL_FALSE, glm::value_ptr(inverse_view_proj));
	}

	void update_solar() {
		//Upload solar information
		glUniform3fv(glGetUniformLocation(_atmosphere_shader.program, "sunDirection"), 1, glm::value_ptr(_sun_direction));
		glm::vec3 sunInverseWavelength = 1.0f / glm::pow(_sun_wavelengths, glm::vec3(4.0f));
		glUniform3fv(glGetUniformLocation(_atmosphere_shader.program, "sunInverseWavelength"), 1, glm::value_ptr(sunInverseWavelength));
		glUniform1f(glGetUniformLocation(_atmosphere_shader.program, "sunIntensity"), _sun_intensity);
	}

	void update_planets() {
		glUniform3fv(glGetUniformLocation(_atmosphere_shader.program, "planetWorldPos"), 1, glm::value_ptr(_planet_world_pos));
		glUniform1f(glGetUniformLocation(_atmosphere_shader.program, "planetRadius"), _planet_radius);
		glUniform1f(glGetUniformLocation(_atmosphere_shader.program, "atmosphereRadius"), _atmosphere_radius);
	}

	glm::vec3 _sun_direction = glm::normalize(glm::vec3(0, 1, 0));
	glm::vec3 _sun_wavelengths = glm::vec3(0.65, 0.57, 0.475);
	float _sun_intensity = 20.0f;
	glm::vec3 _planet_world_pos = glm::vec3(0, 0, 0);
	float _planet_radius = 4.5f;
	float _atmosphere_radius = 1.025f * _planet_radius;
	float _scale_height = 0.25f;
	float _mie_phase_g = -0.999f;
	float _mie_constant = 0.0010f;
	float _rayleigh_constant = 0.0025f;

	Shader _atmosphere_shader = Shader("Shaders/atmosphereshader.vert", "Shaders/atmosphereshader.frag");
};