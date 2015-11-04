#pragma once
#include "..\View\Drawable.h"
#include <GLM\gtc\type_ptr.hpp>
#include <SOIL\SOIL.h>
#include <memory>
class Skybox : public Drawable {
public:
	Skybox() : Drawable() {

		GLuint texid;
		glGenTextures(1, &texid);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texid);

		int w, h, c;
		unsigned char *data = nullptr;

		data = SOIL_load_image("Images/skybox_space_right1.png", &w, &h, &c, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		
		data = SOIL_load_image("Images/skybox_space_left2.png", &w, &h, &c, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		data = SOIL_load_image("Images/skybox_space_top3.png", &w, &h, &c, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		data = SOIL_load_image("Images/skybox_space_bottom4.png", &w, &h, &c, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		data = SOIL_load_image("Images/skybox_space_front5.png", &w, &h, &c, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		data = SOIL_load_image("Images/skybox_space_back6.png", &w, &h, &c, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		_texid = texid;

		_mesh.vertices.push_back({ { -1.0f,  -1.0f, 1.0f } }); //front
		_mesh.vertices.push_back({ { 1.0f,  -1.0f, 1.0f } }); //front
		_mesh.vertices.push_back({ { 1.0f,  1.0f, 1.0f } }); //front
		_mesh.vertices.push_back({ { -1.0f,  1.0f, 1.0f } }); //front

		_mesh.vertices.push_back({ { -1.0f,  -1.0f, -1.0f } }); //back
		_mesh.vertices.push_back({ { 1.0f,  -1.0f, -1.0f } }); //back
		_mesh.vertices.push_back({ { 1.0f,  1.0f, -1.0f } }); //back
		_mesh.vertices.push_back({ { -1.0f,  1.0f, -1.0f } }); //back

		_mesh.indices.push_back(0); _mesh.indices.push_back(1); _mesh.indices.push_back(2); //front
		_mesh.indices.push_back(2); _mesh.indices.push_back(3); _mesh.indices.push_back(0); //front
		_mesh.indices.push_back(3); _mesh.indices.push_back(2); _mesh.indices.push_back(6); //top
		_mesh.indices.push_back(6); _mesh.indices.push_back(7); _mesh.indices.push_back(3); //top
		_mesh.indices.push_back(7); _mesh.indices.push_back(6); _mesh.indices.push_back(5); //back
		_mesh.indices.push_back(5); _mesh.indices.push_back(4); _mesh.indices.push_back(7); //back
		_mesh.indices.push_back(4); _mesh.indices.push_back(5); _mesh.indices.push_back(1); //bottom
		_mesh.indices.push_back(1); _mesh.indices.push_back(0); _mesh.indices.push_back(4); //bottom
		_mesh.indices.push_back(4); _mesh.indices.push_back(0); _mesh.indices.push_back(3); //left
		_mesh.indices.push_back(3); _mesh.indices.push_back(7); _mesh.indices.push_back(4); //left
		_mesh.indices.push_back(1); _mesh.indices.push_back(5); _mesh.indices.push_back(6); //right
		_mesh.indices.push_back(6); _mesh.indices.push_back(2); _mesh.indices.push_back(1); //right

		_mesh.setup_mesh();

		set_shader(_skybox_shader, false);
		
	}

	void draw(const Lighting & lighting, const Camera & camera, double delta_time) override {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _texid);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		_shader->use();
		glm::mat4 skybox_view = camera.get_view();
		skybox_view[3] = glm::vec4(0,0,0,1);
		glUniformMatrix4fv(glGetUniformLocation(_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(skybox_view));
		glUniformMatrix4fv(glGetUniformLocation(_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
		glUniform1i(glGetUniformLocation(_shader->program, "skybox"), 0);
		_mesh.draw(_shader, delta_time);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	};

	void draw_wireframe(const Lighting & lighting, const Camera & camera, double delta_time) override {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _texid);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		_shader->use();
		glUniformMatrix4fv(glGetUniformLocation(_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
		glUniformMatrix4fv(glGetUniformLocation(_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
		glUniform1i(glGetUniformLocation(_shader->program, "skybox"), 0);
		_mesh.draw_wireframe(_shader, delta_time);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	};

private:
	std::shared_ptr<Shader> _skybox_shader = std::make_shared<Shader>("Shaders/skyboxshader.vert", "Shaders/skyboxshader.frag");
	GLuint _texid;
};