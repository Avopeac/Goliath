#pragma once
#include <GLM\gtc\type_ptr.hpp>
#include <SOIL\SOIL.h>
#include <memory>
#include "..\Model\CubeMap.h"
#include "..\Drawable\Cube.h"
#include "..\Input\Input.h"
class Skybox : public Cube {
public:
	Skybox() : Cube() {
		_skybox_cubemap.load_cubemap(
			"Images/skybox_space_right1.png",
			"Images/skybox_space_left2.png",
			"Images/skybox_space_top3.png",
			"Images/skybox_space_bottom4.png",
			"Images/skybox_space_front5.png",
			"Images/skybox_space_back6.png"
			);
		set_shader(_skybox_shader, false);

		//DEBUG
		TwAddSeparator(Input::_tw_bar, "Skybox and Atmosphere", NULL);
		TwAddVarRW(Input::_tw_bar, "Scattering RED", TW_TYPE_FLOAT, &_scattering.r, " min=0.0 max=1.0 step=0.01 ");
		TwAddVarRW(Input::_tw_bar, "Scattering GREEN", TW_TYPE_FLOAT, &_scattering.g, " min=0.0 max=1.0 step=0.01 ");
		TwAddVarRW(Input::_tw_bar, "Scattering BLUE", TW_TYPE_FLOAT, &_scattering.b, " min=0.0 max=1.0 step=0.01 ");
		TwAddVarRW(Input::_tw_bar, "Scattering Overglow", TW_TYPE_FLOAT, &_scattering.a, " min=0.0 max=1.0 step=0.01 ");
		TwAddVarRW(Input::_tw_bar, "Scattering Depth", TW_TYPE_FLOAT, &_max_scattering_depth, " min=0.0 max=1000000.0 step=0.1 ");
		TwAddVarRW(Input::_tw_bar, "Planet Radius", TW_TYPE_FLOAT, &_planet_radius, " min=0.0 max=1000000.0 step=0.1 ");
		TwAddVarRW(Input::_tw_bar, "Atmosphere Radius", TW_TYPE_FLOAT, &_atmosphere_radius, " min=0.0 max=1000000.0 step=0.1 ");
	}

	void draw(const Lighting & lighting, const Camera & camera, double delta_time) override {
		begin_draw_skybox(lighting, camera);
		_mesh.draw(_shader, delta_time);
		end_draw_skybox();
	};

	void draw_wireframe(const Lighting & lighting, const Camera & camera, double delta_time) override {
		begin_draw_skybox(lighting, camera);
		_mesh.draw_wireframe(_shader, delta_time);
		end_draw_skybox();
	};

private:

	void begin_draw_skybox(const Lighting &lighting, const Camera &camera){
		_shader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _skybox_cubemap.texid);
		//Make skybox span infinitely
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		//Zero out translation
		glm::mat4 skybox_view = camera.get_view();
		skybox_view[3] = { 0, 0, 0, 1 };
		glm::vec3 planetPos = glm::vec3(camera.get_view() * glm::vec4(0, 0, 0, 1));
		glUniform3fv(glGetUniformLocation(_shader->program, "planetViewPos"), 1, glm::value_ptr(planetPos));
		glUniform1f(glGetUniformLocation(_shader->program, "planetRadius"), _planet_radius);
		glUniform1f(glGetUniformLocation(_shader->program, "atmosphereRadius"), _atmosphere_radius);
		glUniform4fv(glGetUniformLocation(_shader->program, "scattering"), 1, glm::value_ptr(_scattering));
		glUniform1f(glGetUniformLocation(_shader->program, "viewDepth"), _max_scattering_depth);
		glUniformMatrix4fv(glGetUniformLocation(_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(skybox_view));
		on_first_draw(lighting, camera);
		glUniform1i(glGetUniformLocation(_shader->program, "skybox"), 0);
	}

	void on_first_draw(const Lighting &lighting, const Camera &camera) {
		if (_first_draw) {
			_first_draw = false;
			glUniformMatrix4fv(glGetUniformLocation(_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
			Lighting l = lighting;
			if (l.get_num_lights() > 0) {
				//For each light convert to view space, better here than in shader
				for (unsigned int i = 0; i < l.get_num_lights(); ++i) {
					l.view_space_directions[i] = glm::mat3(camera.get_view()) * l.directions[i];
				}
				glUniform1i(glGetUniformLocation(_shader->program, "lights"), l.get_num_lights());
				glUniform3fv(glGetUniformLocation(_shader->program, "directions"), l.get_num_lights(), &l.view_space_directions[0][0]);
				glUniform3fv(glGetUniformLocation(_shader->program, "intensities"), l.get_num_lights(), &l.intensities[0][0]);
			}
		}
	}

	void end_draw_skybox() {
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	std::shared_ptr<Shader> _skybox_shader = std::make_shared<Shader>("Shaders/skyboxshader.vert", "Shaders/skyboxshader.frag");
	CubeMap _skybox_cubemap;
	bool _first_draw = true;
	glm::vec4 _scattering = glm::vec4(0.1981, 0.4656, 0.8625, 0.75);
	float _max_scattering_depth = 2.0;
	float _atmosphere_radius = 4.5;
	float _planet_radius = 4.0;
};