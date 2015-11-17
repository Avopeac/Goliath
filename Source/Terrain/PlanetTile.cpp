#include "PlanetTile.h"
#include <iostream>
#include "GLM\gtc\type_ptr.hpp"

SimplePlanetHeightSampler PlanetTile::sampler = SimplePlanetHeightSampler(2.0f, 12.0f, 0.8f, 0.0f);

void PlanetTile::generate(const glm::mat4 &translation, const glm::mat4 &scale, const glm::mat4 &rotation)
{
	int x, z;
	std::vector<VertexData> vertex_data;
	float step = 1.0f / _resolution;
	float offset = 0.5f;
	//Set up vertices with the edge case
	for (x = -1; x <= _resolution + 1; ++x) {
		for (z = -1; z <= _resolution + 1; ++z) {
			VertexData current;
			float cx = x * step - offset;
			float cz = z * step - offset;
			current.edge = is_edge(x, z);
			current.vertex.position = glm::vec3(translation * rotation * scale *  glm::vec4(cx, 0, cz, 1.0));
			current.vertex.position = glm::normalize(current.vertex.position);
			float height = sampler.sample(current.vertex.position);
			current.vertex.position = (4.0f + pow(height, 4) * 0.15f) * current.vertex.position; //Pow 4 gives us more exaggerations
			current.vertex.color.r = height; //Save terrain height in red-channel
			vertex_data.push_back(current);
		}
	}

	//Set up indices with the edge case
	int stride = _resolution + 2 + 1;
	for (x = 0; x < _resolution + 2; ++x) {
		for (z = 0; z < _resolution + 2; ++z) {
			_mesh.indices.push_back(x + 1 + z * stride);
			_mesh.indices.push_back(x + (z + 1) * stride);
			_mesh.indices.push_back(x + z * stride);
			_mesh.indices.push_back(x + 1 + (z + 1) * stride);
			_mesh.indices.push_back(x + (z + 1) * stride);
			_mesh.indices.push_back(x + 1 + z * stride);
		}
	}

	//Calculate vertex normals, simple version, only averages over one triangle
	z = (int)_mesh.indices.size();
	for (x = 0; x < z; x += 3) {
		int i1(_mesh.indices[x + 0]);
		int i2(_mesh.indices[x + 1]);
		int i3(_mesh.indices[x + 2]);
		glm::vec3 p1(vertex_data[i1].vertex.position);
		glm::vec3 p2(vertex_data[i2].vertex.position);
		glm::vec3 p3(vertex_data[i3].vertex.position);
		glm::vec3 normal(glm::cross(p2 - p1, p3 - p1));
		vertex_data[i1].vertex.normal += normal;
		vertex_data[i2].vertex.normal += normal;
		vertex_data[i3].vertex.normal += normal;
	}

	for (auto it = vertex_data.begin(); it != vertex_data.end(); ++it) {
		if (it->edge) {
			it->vertex.position *= 0.9f;
		}
		_mesh.vertices.push_back(it->vertex);
	}

	_mesh.setup_mesh();
}

bool PlanetTile::is_edge(int x, int z) {
	return (x == -1) || (z == -1) || (x == _resolution + 1) || (z == _resolution + 1);
}

void PlanetTile::draw(const Camera & camera, double delta_time)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	_shader->use();
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
	//glUniformMatrix4fv(glGetUniformLocation(_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	//glUniformMatrix4fv(glGetUniformLocation(_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
	_mesh.draw(_shader, delta_time);
}

void PlanetTile::draw_wireframe(const Camera & camera, double delta_time)
{
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	_shader->use();
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
	//glUniformMatrix4fv(glGetUniformLocation(_shader->program, "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
	//glUniformMatrix4fv(glGetUniformLocation(_shader->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective()));
	_mesh.draw_wireframe(_shader, delta_time);
}
