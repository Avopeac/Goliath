#include <vector>
#include <GLM\gtc\constants.hpp>
#include <GLM\gtc\type_ptr.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#include "Sphere.h"
#include "..\View\Renderer.h"
Sphere::Sphere(const glm::vec3 & origin, double radius) : Primitive(origin), Drawable(), _radius(radius) {}



void Sphere::generate_mesh(unsigned int latitudes, unsigned int longitudes) {
	//TODO: Something is not right
	_mesh.vertices.reserve(2 + latitudes * (longitudes + 1));

	//Positions
	_mesh.vertices.push_back(glm::vec3(0, 1, 0) * (float)_radius);
	for (unsigned int lat = 0; lat < latitudes; ++lat) {
		float a1 = glm::pi<float>() * (float)(lat + 1) / (latitudes + 1);
		float sin1 = glm::cos(a1);
		float cos1 = glm::sin(a1);
		for (unsigned int lon = 0; lon <= longitudes; ++lon) {
			float a2 = glm::two_pi<float>() * (float)(lon == longitudes ? 0 : lon) / longitudes;
			float sin2 = glm::sin(a2);
			float cos2 = glm::cos(a2);
			_mesh.vertices.push_back(glm::vec3(sin1 * cos2, cos1, sin1 * sin2) * (float)_radius);
		}
	}
	_mesh.vertices.push_back(glm::vec3(0, -1, 0) * (float)_radius);

	//Normals
	for (auto v = _mesh.vertices.begin(); v != _mesh.vertices.end(); ++v) {
		(*v).normal = glm::normalize((*v).position);
	}

	//Texcoords
	_mesh.vertices.front().texcoord = glm::vec2(0, 1);
	_mesh.vertices.back().texcoord = glm::vec2(0, 0);
	for (unsigned int lat = 0; lat < latitudes; ++lat) {
		for (unsigned int lon = 0; lon < longitudes; ++lon) {
			_mesh.vertices.at(lon + lat * (longitudes + 1) + 1).texcoord = glm::vec2((float)lon / longitudes, 1.0f - (float)(lat + 1) / (latitudes + 1));
		}
	}

	_mesh.indices.reserve(_mesh.vertices.size() * 2 * 3);
	//Top
	for (unsigned int lon = 0; lon < longitudes; ++lon) {
		_mesh.indices.push_back(lon + 2);
		_mesh.indices.push_back(lon + 1);
		_mesh.indices.push_back(0);
	}
	//Middle
	for (unsigned int lat = 0; lat < latitudes - 1; ++lat) {
		for (unsigned int lon = 0; lon < longitudes; ++lon) {
			unsigned int current = lon + lat * (longitudes + 1) + 1;
			unsigned int next = current + longitudes + 1;
			_mesh.indices.push_back(current);
			_mesh.indices.push_back(current + 1);
			_mesh.indices.push_back(next + 1);
			_mesh.indices.push_back(current);
			_mesh.indices.push_back(next + 1);
			_mesh.indices.push_back(next);
		}
	}

	//Bottom
	unsigned int count = (unsigned int)_mesh.vertices.size();
	for (unsigned int lon = 0; lon < longitudes; ++lon) {
		_mesh.indices.push_back(count);
		_mesh.indices.push_back(count - lon + 2);
		_mesh.indices.push_back(count - lon + 1);
	}

	_mesh.setup_mesh();
}

void Sphere::draw(const Camera &camera, double delta_time) {
	Shader &shader = _mesh.get_shader();
	shader.use();
	glm::mat4 m(1);
	m = glm::translate(m, _origin);
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(m));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
	_mesh.draw(camera, delta_time);
}

void Sphere::draw_wireframe(const Camera &camera, double delta_time) {
	Shader &shader = _mesh.get_shader();
	shader.use();
	glm::mat4 m(1);
	m = glm::translate(m, _origin);
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(m));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
	_mesh.draw_wireframe(camera, delta_time);
}
