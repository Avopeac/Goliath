#pragma once
#include <GLM\gtc\type_ptr.hpp>
#include <memory>
#include "Drawable/Cube.h"
#include "Model/CubeMap.h"

class Skybox : public Cube {
public:
	Skybox() = delete;
	Skybox(const std::string& right_path, const std::string& left_path, 
		const std::string& top_path, const std::string& bottom_path, 
		const std::string& front_path, const std::string& back_path) : Cube() {

		_skybox_cubemap.load_cubemap(right_path.c_str(), left_path.c_str(), top_path.c_str(),
			bottom_path.c_str(), front_path.c_str(), back_path.c_str());

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
