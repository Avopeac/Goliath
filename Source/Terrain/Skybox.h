#pragma once
#include <GLM\gtc\type_ptr.hpp>
#include <SOIL\SOIL.h>
#include <memory>
#include "..\Model\CubeMap.h"
#include "..\Drawable\Cube.h"

#ifndef RIGHT_SKYBOX_IMAGE
#define RIGHT_SKYBOX_IMAGE "Images/skybox_space_right1.png"
#endif 

#ifndef LEFT_SKYBOX_IMAGE
#define LEFT_SKYBOX_IMAGE "Images/skybox_space_left2.png"
#endif 

#ifndef TOP_SKYBOX_IMAGE
#define TOP_SKYBOX_IMAGE "Images/skybox_space_top3.png"
#endif 

#ifndef BOTTOM_SKYBOX_IMAGE
#define BOTTOM_SKYBOX_IMAGE "Images/skybox_space_bottom4.png"
#endif 

#ifndef FRONT_SKYBOX_IMAGE
#define FRONT_SKYBOX_IMAGE "Images/skybox_space_front5.png"
#endif 

#ifndef BACK_SKYBOX_IMAGE
#define BACK_SKYBOX_IMAGE "Images/skybox_space_back6.png"
#endif 

class Skybox : public Cube {
public:
	Skybox() : Cube() {
		_skybox_cubemap.load_cubemap(RIGHT_SKYBOX_IMAGE, LEFT_SKYBOX_IMAGE, TOP_SKYBOX_IMAGE, BOTTOM_SKYBOX_IMAGE, FRONT_SKYBOX_IMAGE, BACK_SKYBOX_IMAGE );
		std::shared_ptr<Shader> skybox_shader = ShaderStore::instance().get_shader_from_store(SKYBOX_SHADER_PATH);
		set_shader(skybox_shader);
	}

	void draw(const Camera & camera, double delta_time) override {
		begin_draw_skybox(camera);
		_mesh.draw(_shader, delta_time);
		end_draw_skybox();
	};

	void draw_wireframe(const Camera & camera, double delta_time) override {
		begin_draw_skybox(camera);
		_mesh.draw_wireframe(_shader, delta_time);
		end_draw_skybox();
	};

private:

	void begin_draw_skybox(const Camera &camera){
		_shader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _skybox_cubemap.texid);
		//Make skybox span infinitely
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
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
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	CubeMap _skybox_cubemap;
	bool _first_draw = true;
};