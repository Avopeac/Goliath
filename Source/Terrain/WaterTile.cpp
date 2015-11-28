#include "WaterTile.h"
#include <iostream>
#include <GLM/gtc/type_ptr.hpp>
#include <Thread/Message.h>
#include <Thread/MessageSystem.h>

class WaterTile::WaterTileMessage : public Message {
public:
	explicit WaterTileMessage(WaterTile *tile) : _tile(tile) {}
	virtual void process() override { _tile->generate(); }
private:
	WaterTile *_tile;
};

class WaterTile::VertexData {
public:
	VertexData() {}
	~VertexData() {}
	glm::dvec3 parent_position;
	glm::dvec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 uv;
	bool edge = false;
};

WaterTile::WaterTile(const glm::dmat4 &translation, const glm::dmat4 &scale, const glm::dmat4 &rotation, double radii, std::shared_ptr<Shader> shader)
	: Drawable(), _radii(radii), _transform(translation * rotation * scale)  {
	set_shader(shader);
	_message_ref = MessageSystem::instance().post(std::make_shared<WaterTileMessage>(this));
}

void WaterTile::generate() {
	int x, z;
	glm::dvec3 max(glm::dvec3(std::numeric_limits<double>().min()));
	glm::dvec3 min(glm::dvec3(std::numeric_limits<double>().max()));
	//Reserve vertex data
	std::vector<VertexData> vertex_data;
	vertex_data.reserve((2 + WATER_TILE_RESOLUTION) * (2 + WATER_TILE_RESOLUTION));
	for (x = -1; x <= WATER_TILE_RESOLUTION + 1; ++x) {
		for (z = -1; z <= WATER_TILE_RESOLUTION + 1; ++z) {
			//Cx, cz are normalized coordinates with 0.5, 0.5 in the center
			double cx(x * WATER_TILE_INV_RESOLUTION - WATER_TILE_OFFSET);
			double cz(z * WATER_TILE_INV_RESOLUTION- WATER_TILE_OFFSET);
			VertexData current;
			current.edge = is_edge(x, z);
			//Transform point to unit sphere and scale
			current.position = glm::normalize(glm::dvec3(_transform *  glm::dvec4(cx, 0, cz, 1.0)));
			current.position = _radii * current.position;
			current.uv = { cx + WATER_TILE_OFFSET, cz + WATER_TILE_OFFSET };
			//Some height-based color fraction
			vertex_data.push_back(current);
			//Find max points
			if (max.x < current.position.x) { max.x = current.position.x; }
			if (max.y < current.position.y) { max.y = current.position.y; }
			if (max.z < current.position.z) { max.z = current.position.z; }
			//Find min points
			if (min.x > current.position.x) { min.x = current.position.x; }
			if (min.y > current.position.y) { min.y = current.position.y; }
			if (min.z > current.position.z) { min.z = current.position.z; }
		}
	}

	//Set up true aabb
	_center = (max + min) * 0.5;
	_extents = (max - min) * 0.5;

	//Set up indices with the edge case
	int stride = WATER_TILE_RESOLUTION + 2 + 1;
	for (x = 0; x < WATER_TILE_RESOLUTION + 2; ++x) {
		for (z = 0; z < WATER_TILE_RESOLUTION + 2; ++z) {
			mesh.indices.push_back(x + 1 + z * stride);
			mesh.indices.push_back(x + (z + 1) * stride);
			mesh.indices.push_back(x + z * stride);
			mesh.indices.push_back(x + 1 + (z + 1) * stride);
			mesh.indices.push_back(x + (z + 1) * stride);
			mesh.indices.push_back(x + 1 + z * stride);
		}
	}

	//Calculate vertex normals, simple version, only averages over one triangle
	z = (int)mesh.indices.size();
	for (x = 0; x < z; x += 3) {
		int i1(mesh.indices[x + 0]);
		int i2(mesh.indices[x + 1]);
		int i3(mesh.indices[x + 2]);
		glm::dvec3 normal(glm::cross(vertex_data[i2].position - vertex_data[i1].position,
			vertex_data[i3].position - vertex_data[i1].position));
		vertex_data[i1].normal += normal;
		vertex_data[i2].normal += normal;
		vertex_data[i3].normal += normal;
	}

	// "Bend down" skirts
	for (auto it = vertex_data.begin(); it != vertex_data.end(); ++it) {
		if (it->edge) { it->position *= 0.95f; }
		mesh.vertices.push_back(Vertex(it->position - _center, it->normal, it->uv, it->color));
	}

	_setup_done = true;
}

void WaterTile::predraw(const Camera &camera) {
	if (_message_ref != -1) {
		auto message(MessageSystem::instance().get(_message_ref));
		// Setup mesh once
		if (message) {
			mesh.setup_mesh();
			_message_ref = -1;
			_setup_done = true;
		}
		else {
			std::cerr << "WaterTile: No return message from MessageSystem" << std::endl;
			return;
		}
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	_shader->use();
	glm::dmat4 relative_to_center(camera.get_dview());
	glm::dvec4 center_eye(relative_to_center * glm::dvec4(_center, 1));
	relative_to_center[3] = center_eye;
	relative_to_center = camera.get_dprojection() * relative_to_center;
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "mvp"), 1, GL_FALSE, glm::value_ptr(glm::mat4(relative_to_center)));
}

void WaterTile::draw(const Camera & camera, double delta_time) {
	if (!_setup_done) { return; }
	predraw(camera);
	mesh.draw(_shader, delta_time);
}

void WaterTile::draw_wireframe(const Camera & camera, double delta_time) {
	if (!_setup_done) { return; }
	predraw(camera);
	mesh.draw_wireframe(_shader, delta_time);
}
