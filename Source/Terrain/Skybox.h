#pragma once
#include <GLM\gtc\type_ptr.hpp>
#include <SOIL\SOIL.h>
#include <memory>
#include "..\Model\CubeMap.h"
#include "..\Drawable\Cube.h"
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
		glUniformMatrix4fv(glGetUniformLocation(_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(skybox_view));
		if (_first_draw) {
			_first_draw = false;
			glUniformMatrix4fv(glGetUniformLocation(_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
		}
		glUniform1i(glGetUniformLocation(_shader->program, "skybox"), 0);
	}

	void end_draw_skybox() {
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	std::shared_ptr<Shader> _skybox_shader = std::make_shared<Shader>("Shaders/skyboxshader.vert", "Shaders/skyboxshader.frag");
	CubeMap _skybox_cubemap;
	bool _first_draw = true;
};