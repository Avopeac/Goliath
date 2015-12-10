#include "PlanetTile.h"
#include <iostream>
#include <GLM/gtc/type_ptr.hpp>
#include <Thread/Message.h>
#include <Thread/MessageSystem.h>
#include "View/ShaderStore.h"

SimplePlanetHeightSampler PlanetTile::sampler = SimplePlanetHeightSampler(2.0, 24.0, 0.0999, 0.4);

class PlanetTile::PlanetTileMessage : public Message {
public:
	explicit PlanetTileMessage(PlanetTile *tile) : _tile(tile) {}
	virtual void process() override { _tile->generate(); }
private:
	PlanetTile *_tile;
};

class PlanetTile::VertexData {
public:
	VertexData() {}
	~VertexData() {}
	glm::dvec3 position;
	glm::vec3 normal; 
	glm::vec3 extra;
	glm::vec3 color;
	glm::dvec2 uv;
	bool edge = false;
};

PlanetTile::PlanetTile(const glm::dmat4 &translation, const glm::dmat4 &scale, const glm::dmat4 &rotation, double radii, std::shared_ptr<Shader> shader)
	: Drawable(), _radii(radii), _transform(translation * rotation * scale), _inv_outer_radii(1.0 / (1.025 * _radii)) {
	set_shader(shader);
	_message_ref = MessageSystem::instance().post(std::make_shared<PlanetTileMessage>(this));
}

void PlanetTile::generate() {
	int x, z;
	glm::dvec3 max(glm::dvec3(std::numeric_limits<double>().min()));
	glm::dvec3 min(glm::dvec3(std::numeric_limits<double>().max()));
	//Reserve vertex data
	std::vector<VertexData> vertex_data;
	//vertex_data.reserve((2 + PLANET_TILE_RESOLUTION) * (2 + PLANET_TILE_RESOLUTION));
	for (x = -1; x <= PLANET_TILE_RESOLUTION + 1; ++x) {
		for (z = -1; z <= PLANET_TILE_RESOLUTION + 1; ++z) {
			//Cx, cz are normalized coordinates with 0.5, 0.5 in the center
			double cx(x * PLANET_TILE_INV_RESOLUTION - PLANET_TILE_OFFSET);
			double cz(z * PLANET_TILE_INV_RESOLUTION - PLANET_TILE_OFFSET);
			double height(1);
			VertexData current;
			current.edge = is_edge(x, z);
			//Transform point to unit sphere and scale
			current.position = glm::normalize(glm::dvec3(_transform *  glm::dvec4(cx, 0, cz, 1.0)));
			current.uv = glm::vec2(cx + PLANET_TILE_OFFSET, cz + PLANET_TILE_OFFSET);
			current.normal = { 0, 0, 0 };
			height = height_scaler(current.position);
			current.position = (_radii + PLANET_TILE_MAX_MOUNTAIN_HEIGHT * height) * current.position;
			current.extra = current.position * _inv_outer_radii;

			//Some height-based color fraction
			current.color.r = static_cast<float>(height);
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
	int stride = PLANET_TILE_RESOLUTION + 2 + 1;
	for (x = 0; x < PLANET_TILE_RESOLUTION + 2; ++x) {
		for (z = 0; z < PLANET_TILE_RESOLUTION + 2; ++z) {
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
		glm::dvec3 normal(glm::cross(vertex_data[i1].position - vertex_data[i2].position,
			vertex_data[i3].position - vertex_data[i2].position));
		normal.y = -normal.y;
		vertex_data[i1].normal += normal;
		vertex_data[i2].normal += normal;
		vertex_data[i3].normal += normal;
	}

	// "Bend down" skirts
	for (auto it = vertex_data.begin(); it != vertex_data.end(); ++it) {
		if (it->edge) { it->position *= 0.99f; } //Keep as small as possible to reduce fragment computations
		mesh.vertices.push_back(Vertex(it->position - _center, it->normal, it->uv, it->color, it->extra));
	}

	_setup_done = true;
}

void PlanetTile::predraw(const Camera &camera) {
	if (_message_ref != -1) {
		auto message(MessageSystem::instance().get(_message_ref));
		// Setup mesh once
		if (message) {
			mesh.setup_mesh();
			_message_ref = -1;
			_setup_done = true;
		}
		else {
			std::cerr << "PlanetTile: No return message from MessageSystem" << std::endl;
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
	glm::mat4 downcast_rtc(relative_to_center);
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "mv"), 1, GL_FALSE, glm::value_ptr(downcast_rtc));
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "mvp"), 1, GL_FALSE, glm::value_ptr(camera.get_fprojection() * downcast_rtc));
}

void PlanetTile::draw(const Camera & camera, double delta_time) {
	if (!_setup_done) { return; }
	predraw(camera);
	mesh.draw(_shader, delta_time);
}

void PlanetTile::draw_wireframe(const Camera & camera, double delta_time) {
	if (!_setup_done) { return; }
	predraw(camera);
	mesh.draw_wireframe(_shader, delta_time);
}
