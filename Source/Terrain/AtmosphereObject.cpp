#include "AtmosphereObject.h"
#include <GLM/gtc/type_ptr.hpp>
#include "Terrain/DayNight.h"

void AtmosphereObject::begin_draw(const Camera &camera) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glm::dvec3 cam_pos = camera.get_deye() *_factor;
	double h = length(cam_pos);
	double h2 = h * h;

	//Shader LOD for atmosphere
	if (glm::length(cam_pos) < 1.0) {
		set_shader(sky_from_atmosphere_shader);
	}
	else {
		set_shader(sky_from_space_shader);
	}
	_shader->use();

	//Relative to eye rendering
	glm::dmat4 rte(camera.get_dview());
	rte[3] = glm::dvec4(0, 0, 0, 1);
	glm::mat4 rte_gpu(camera.get_dprojection() * rte);
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "mvp"), 1, GL_FALSE, glm::value_ptr(rte_gpu));

	//Convert camera position to two floats
	glm::vec3 high_camera, low_camera;
	double_vec_to_float_vecs(camera.get_deye(), high_camera, low_camera);
	glUniform3fv(glGetUniformLocation(_shader->program, "highCamera"), 1, glm::value_ptr(high_camera));
	glUniform3fv(glGetUniformLocation(_shader->program, "lowCamera"), 1, glm::value_ptr(low_camera));
	glUniform3fv(glGetUniformLocation(_shader->program, "cameraPos"), 1, glm::value_ptr(glm::vec3(cam_pos)));
	glUniform1f(glGetUniformLocation(_shader->program, "cameraHeight"), h);
	glUniform1f(glGetUniformLocation(_shader->program, "cameraHeight2"), h2);

	double ir_scaled = _radius * _factor;
	double or_scaled = _atmosphere_radius * _factor;
	double or_scaled2 = or_scaled * or_scaled;

	double scale = 1.0 / (or_scaled - ir_scaled);
	double scale_depth = 0.25;
	double scale_over_depth = scale / scale_depth;

	const double kr = 0.0025;
	const double km = 0.001;
	const double e_sun = 20.0;
	const double f_pi = 4.0 * glm::pi<double>();
	const double g = -0.99;

	glm::vec3 inv_wave_length = glm::vec3(0.65, 0.57, 0.475);
	inv_wave_length = 1.0f / glm::pow(inv_wave_length, glm::vec3(4.0f));
	glUniform1f(glGetUniformLocation(_shader->program, "innerRadius"), ir_scaled);
	glUniform1f(glGetUniformLocation(_shader->program, "outerRadius"), or_scaled);
	glUniform1f(glGetUniformLocation(_shader->program, "outerRadius2"), or_scaled2);
	glUniform1f(glGetUniformLocation(_shader->program, "scale"), scale);
	glUniform1f(glGetUniformLocation(_shader->program, "scaleDepth"), scale_depth);
	glUniform1f(glGetUniformLocation(_shader->program, "scaleOverDepth"), scale_over_depth);
	glUniform1f(glGetUniformLocation(_shader->program, "Kr4Pi"), kr * f_pi);
	glUniform1f(glGetUniformLocation(_shader->program, "Km4Pi"), km * f_pi);
	glUniform1f(glGetUniformLocation(_shader->program, "KmEsun"), km * e_sun);
	glUniform1f(glGetUniformLocation(_shader->program, "KrEsun"), kr * e_sun);
	glUniform1f(glGetUniformLocation(_shader->program, "g"), g);
	glUniform1f(glGetUniformLocation(_shader->program, "g2"), g * g);
	glUniform3fv(glGetUniformLocation(_shader->program, "invWaveLength"), 1, glm::value_ptr(inv_wave_length));
	glUniform3fv(glGetUniformLocation(_shader->program, "lightDir"), 1, glm::value_ptr(DayNight::instance().get_sun()));
}

void AtmosphereObject::end_draw() {
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void AtmosphereObject::draw(const Camera & camera, double delta_time) {
	begin_draw(camera);
	mesh.draw(_shader, delta_time);
	end_draw();
}

void AtmosphereObject::draw_wireframe(const Camera & camera, double delta_time) {
	begin_draw(camera);
	mesh.draw_wireframe(_shader, delta_time);
	end_draw();
}
