#include <vector>
#include <iostream>
#include <GLM\gtc\constants.hpp>
#include <GLM\gtc\type_ptr.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#include <AntTweakBar\AntTweakBar.h>
#include "Sphere.h"
#include "..\Input\Input.h"
#include "..\View\Renderer.h"
Sphere::Sphere(const glm::vec3 & origin, double radius) : Primitive(origin), Drawable(), _radius(radius) {
	//Set up material
	_material.albedo = glm::vec3(0.5, 0.01, 0.01);
	_material.absorption = 0.0f;
	_material.gaussian = 1.0f;
	_material.refraction = 16.0f;
	//Generate mesh
	generate_mesh(25, 25);
	set_shader(Renderer::instance().get_standard_shader(), true);
	//Debug
	TwAddSeparator(Input::_tw_bar, "Sphere BRDF", NULL);
	TwAddVarRW(Input::_tw_bar, "Roughness", TW_TYPE_FLOAT, &_material.roughness, " min=0 max=1 step=0.01 ");
	TwAddVarRW(Input::_tw_bar, "Gaussian", TW_TYPE_FLOAT, &_material.gaussian, " min=0 max=1 step=0.01 ");
	TwAddVarRW(Input::_tw_bar, "Absorption", TW_TYPE_FLOAT, &_material.absorption, " min=0 max=1 step=0.01 ");
	TwAddVarRW(Input::_tw_bar, "Refraction", TW_TYPE_FLOAT, &_material.refraction, " min=0 max=20 step=0.01 ");
}

void Sphere::generate_mesh(unsigned int latitudes, unsigned int longitudes) {
	//Vertices
	_mesh.vertices.reserve((latitudes + 1) * (longitudes + 1));
	for (unsigned int lat = 0; lat <= latitudes; ++lat) {
		float theta = lat * glm::pi<float>() / latitudes;
		float sin_theta = glm::sin(theta);
		float cos_theta = glm::cos(theta);
		for (unsigned int lon = 0; lon <= longitudes; ++lon) {
			float phi = lon * glm::two_pi<float>() / longitudes;
			float sin_phi = glm::sin(phi);
			float cos_phi = glm::cos(phi);
			glm::vec3 r = glm::vec3(cos_phi * sin_theta, cos_theta, sin_phi * sin_theta);
			glm::vec2 uv = glm::vec2(1.0f - (lon / longitudes), 1.0f - (lat / latitudes));
			_mesh.vertices.push_back(Vertex(r, r, uv));
		}
	}
	//Indices
	_mesh.indices.reserve(latitudes * longitudes * 6);
	for (unsigned int lat = 0; lat < latitudes; ++lat) {
		for (unsigned int lon = 0; lon < longitudes; ++lon) {
			unsigned int first = (lat * (longitudes + 1)) + lon;
			unsigned int second = first + longitudes + 1;
			_mesh.indices.push_back(first + 1);
			_mesh.indices.push_back(second);
			_mesh.indices.push_back(first);
			_mesh.indices.push_back(second);
			_mesh.indices.push_back(first + 1);
			_mesh.indices.push_back(second + 1);
		}
	}
	_mesh.setup_mesh();
}

void Sphere::draw(const Lighting &lighting, const Camera &camera, double delta_time) {
	//Set up model matrix
	_model = glm::mat4();
	_model = glm::scale(_model, glm::vec3((float)_radius));
	_model = glm::translate(_model, _origin);
	_shader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	//Enable alpha blending
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUniform3fv(glGetUniformLocation(_shader->program, "albedo"), 1, glm::value_ptr(_material.albedo));
	glUniform1f(glGetUniformLocation(_shader->program, "roughness"), _material.roughness);
	glUniform1f(glGetUniformLocation(_shader->program, "gaussian"), _material.gaussian);
	glUniform1f(glGetUniformLocation(_shader->program, "absorption"), _material.absorption);
	glUniform1f(glGetUniformLocation(_shader->program, "refraction"), _material.refraction);
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "model"), 1, GL_FALSE, glm::value_ptr(_model));
	_mesh.draw(_shader, delta_time);
}

void Sphere::draw_wireframe(const Lighting &lighting, const Camera &camera, double delta_time) {
	//Set up model matrix
	_model = glm::mat4();
	_model = glm::scale(_model, glm::vec3((float)_radius));
	_model = glm::translate(_model, _origin);
	_shader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "model"), 1, GL_FALSE, glm::value_ptr(_model));
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
	_mesh.draw_wireframe(_shader, delta_time);
}
