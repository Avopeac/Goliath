#include "PlanetTile.h"
#include <iostream>
#include <GLM/gtc/type_ptr.hpp>
#include <Thread/Message.h>
#include <Thread/MessageSystem.h>
SimplePlanetHeightSampler PlanetTile::sampler = SimplePlanetHeightSampler(2.0, 25.0, 0.035, 0.7);

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
	//glm::dvec3 parent_position;
	glm::dvec3 position;
	glm::vec3 normal; 
	glm::vec3 sphere_normal;
	glm::vec3 color;
	glm::vec2 uv;
	bool edge = false;
};

PlanetTile::PlanetTile(const glm::dmat4 &translation, const glm::dmat4 &scale, const glm::dmat4 &rotation, double radii, std::shared_ptr<Shader> shader)
	: Drawable(), _radii(radii), _transform(translation * rotation * scale)  {
	set_shader(shader);
	_message_ref = MessageSystem::instance().post(std::make_shared<PlanetTileMessage>(this));
}

void PlanetTile::generate() {
	int x, z;
	glm::dvec3 max(glm::dvec3(std::numeric_limits<double>().min()));
	glm::dvec3 min(glm::dvec3(std::numeric_limits<double>().max()));
	//Reserve vertex data
	std::vector<VertexData> vertex_data;
	vertex_data.reserve((2 + PLANET_TILE_RESOLUTION) * (2 + PLANET_TILE_RESOLUTION));
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
			current.sphere_normal = current.position;
			height = height_scaler(current.position);
			current.position = height * current.position;
			current.uv = { cx + PLANET_TILE_OFFSET, cz + PLANET_TILE_OFFSET };
			//Some height-based color fraction
			current.color.r = static_cast<float>((height - _radii) * PLANET_TILE_MAX_MOUNTAIN_INV_HEIGHT);
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
		glm::dvec3 normal(glm::cross(vertex_data[i2].position - vertex_data[i1].position,
			vertex_data[i3].position - vertex_data[i1].position));
		vertex_data[i1].normal += normal;
		vertex_data[i2].normal += normal;
		vertex_data[i3].normal += normal;
	}

	// Set up parent positions
	/*for (x = -1; x <= _resolution + 1; ++x) {
		for (z = -1; z <= _resolution + 1; ++z) {
			set_parent_position(x, z, trans);
		}
	}*/

	// "Bend down" skirts
	for (auto it = vertex_data.begin(); it != vertex_data.end(); ++it) {
		if (it->edge) { it->position *= 0.9999f; } //Keep as small as possible to reduce fragment computations
		mesh.vertices.push_back(Vertex(it->position - _center, it->normal, it->uv, it->color, it->sphere_normal));
	}

	_setup_done = true;
}


/*void PlanetTile::morph_vertices(double alpha) {
	for (int i = 0; i < vertex_data.size(); i++) {
		if (!vertex_data[i].edge) {
			mesh.vertices[i].position = vertex_data[i].parent_position * (1.0 - alpha) + vertex_data[i].position * alpha; // glm::vec3(0,0,0)
		}
		//if (i == 11) { std::cout << "vertex_data[i].vertex.position " << vertex_data[i].vertex.position.x << "  " << vertex_data[i].vertex.position.y << "  " << vertex_data[i].vertex.position.z << std::endl; }
	}
	mesh.update_vertices();
}*/

/*void PlanetTile::set_parent_position(int x, int z, const glm::dmat4 &transform) {
	double step = 1.0 / _resolution;
	double offset = 0.5;
	glm::dvec3 parent_position(0);
	double cx = x * step - offset;
	double cz = z * step - offset;
	int counter = 0;
	int current_idx = (z + 1) + (x + 1) * (_resolution + 3);
	// If it's an even vertex then parent pos is same as the vertex pos
	if (!((x + 1 + z + 1) % 2)) {
		parent_position = vertex_data[current_idx].position;
		counter++;
	}
	// Else get the average of the near vertices
	else {
		int start_z = glm::max(-1, z - 1);
		int end_z = glm::min(z + 1, (_resolution + 3)); // + 2 cause actual amount of vertices are _res + 1 and then + 1 for left / upper skirt
		int start_x = glm::max(-1, x - 1);
		int end_x = glm::min(x + 1, (_resolution + 3));
		for (int x_idx = start_x; x_idx <= end_x; x_idx++) {
			if (x_idx == x) { continue; }
			for (int z_idx = start_z; z_idx <= end_z; z_idx++) {
				if (z_idx == z) { continue; }
				counter++;
				cx = x_idx * step - offset;
				cz = z_idx * step - offset;
				glm::dvec3 tmp_pos;
				tmp_pos = glm::dvec3(transform *  glm::dvec4(cx, 0, cz, 1.0));
				tmp_pos = glm::normalize(tmp_pos);
				parent_position += height_scaler(tmp_pos) * tmp_pos;
			}
		}
	}

	vertex_data[current_idx].parent_position = parent_position / (double)counter;
	//std::cout << "parent: " << vertex_data[current_idx].parent_position.x << "  " << vertex_data[current_idx].parent_position.y << "  " << vertex_data[current_idx].parent_position.z << "  " << std::endl;
	//std::cout << "real: " << vertex_data[current_idx].own_position.x << "  " << vertex_data[current_idx].own_position.y << "  " << vertex_data[current_idx].own_position.z << "  " << std::endl;
	//Calculate vertex normals, simple version, only averages over one triangle
}*/

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
