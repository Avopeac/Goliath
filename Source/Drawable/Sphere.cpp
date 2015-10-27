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
	_model = glm::scale(_model, glm::vec3((float)_radius));
	_model = glm::translate(_model, _origin);
	//TO BE REMOVED, JUST TESTING TWEAKBAR
	roughness = 0.2f;
	gaussian = 0.2f;
	reflectance = 1.0f;
	distribution = 0;
	diffuseColor = glm::vec3(0.3, 0, 0);
	specularColor = glm::vec3(1.0, 0.9, 0.95);
	TwAddVarRW(Input::_tw_bar, "Roughness", TW_TYPE_FLOAT, &roughness, " min=0 max=1 step=0.01 ");
	TwAddVarRW(Input::_tw_bar, "Distribution", TW_TYPE_INT32, &distribution, " min=0 max=1 step=1 ");
	TwAddVarRW(Input::_tw_bar, "Gaussian", TW_TYPE_FLOAT, &gaussian, " min=0 max=1 step=0.01 ");
	TwAddVarRW(Input::_tw_bar, "Reflectance", TW_TYPE_FLOAT, &reflectance, " min=0 max=1 step=0.01 ");
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

void Sphere::draw(const Camera &camera, double delta_time) {
		_shader.use();
		glUniform3fv(glGetUniformLocation(_shader.program, "diffuseColor"), 1, glm::value_ptr(diffuseColor));
		glUniform3fv(glGetUniformLocation(_shader.program, "specularColor"), 1, glm::value_ptr(specularColor));
		glUniform1f(glGetUniformLocation(_shader.program, "roughness"), roughness);
		glUniform1f(glGetUniformLocation(_shader.program, "gaussian"), gaussian);
		glUniform1f(glGetUniformLocation(_shader.program, "reflectance"), reflectance);
		glUniform1i(glGetUniformLocation(_shader.program, "distribution"), distribution);
		glUniformMatrix4fv(glGetUniformLocation(_shader.program, "model"), 1, GL_FALSE, glm::value_ptr(_model));
		glUniformMatrix4fv(glGetUniformLocation(_shader.program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
		glUniformMatrix4fv(glGetUniformLocation(_shader.program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
		_mesh.draw(_shader, delta_time);
}

void Sphere::draw_wireframe(const Camera &camera, double delta_time) {
	_shader.use();
	glUniformMatrix4fv(glGetUniformLocation(_shader.program, "model"), 1, GL_FALSE, glm::value_ptr(_model));
	glUniformMatrix4fv(glGetUniformLocation(_shader.program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	glUniformMatrix4fv(glGetUniformLocation(_shader.program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
	_mesh.draw_wireframe(_shader, delta_time);
}
