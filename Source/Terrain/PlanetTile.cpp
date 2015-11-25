#include "PlanetTile.h"
#include <iostream>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtx/transform.hpp>
#include <Thread/Message.h>
#include <Thread/MessageSystem.h>

SimplePlanetHeightSampler PlanetTile::sampler = SimplePlanetHeightSampler(2.0f, 24.0f, 0.9f, 0.0f);

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
	Vertex vertex;
	bool edge = false;
	bool skirt = false;
	glm::vec3 own_position;
	glm::vec3 parent_position;
};

PlanetTile::PlanetTile(const glm::mat4 &translation, const glm::mat4 &scale, const glm::mat4 &rotation)
	: Drawable(), _translation(translation), _scale(scale), _rotation(rotation) {
	set_shader(ShaderStore::instance().get_shader_from_store(GROUND_SHADER_PATH));
	_center = glm::vec3(_translation * _rotation * _scale * glm::vec4(0.5, 0, 0.5, 1));
}

PlanetTile::PlanetTile(const glm::mat4 &translation, const glm::mat4 &scale, const glm::mat4 &rotation, std::shared_ptr<Shader> shader)
	: PlanetTile(translation, scale, rotation) {
	set_shader(shader);
	_message_ref = MessageSystem::instance().post(std::make_shared<PlanetTileMessage>(this));
}

void PlanetTile::generate()
{
	int x, z;
	float step = 1.0f / _resolution;
	float offset = 0.5f;
	glm::mat4 trans = _translation * _rotation * _scale;

	//Set up vertices with the edge case
	for (x = -1; x <= _resolution + 1; ++x) {
		for (z = -1; z <= _resolution + 1; ++z) {
			VertexData current;
			float cx = x * step - offset;
			float cz = z * step - offset;
			current.edge = is_edge(x, z);
			current.vertex.position = glm::vec3(trans *  glm::vec4(cx, 0, cz, 1.0));
			current.vertex.position = glm::normalize(current.vertex.position);
			float height = sampler.sample(current.vertex.position);
			current.vertex.position = (2500.0f + height * 0.1f) * current.vertex.position; //Pow 4 gives us more exaggerations
			current.vertex.texcoord = { cx + offset, cz + offset };
			current.vertex.color.r = height;
			current.own_position = current.vertex.position;
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


	// Set up parent positions
	for (x = -1; x <= _resolution + 1; ++x) {
		for (z = -1; z <= _resolution + 1; ++z) {
			set_parent_position(x, z, trans);
		}
	}

	// "Bend down" skirts
	for (auto it = vertex_data.begin(); it != vertex_data.end(); ++it) {
		if (it->edge) {
			it->vertex.position *= 0.95f;
		}
		_mesh.vertices.push_back(it->vertex);
	}

	_setup_done = true;
}


void PlanetTile::morph_vertices(float alpha) {
	for (int i = 0; i < vertex_data.size(); i++) {
		if (!vertex_data[i].edge) {
			_mesh.vertices[i].position = vertex_data[i].parent_position *(1.0f - alpha) + vertex_data[i].own_position*alpha; // glm::vec3(0,0,0)
		}
		/*if (i == 11) {
			std::cout << "vertex_data[i].vertex.position " << vertex_data[i].vertex.position.x << "  " << vertex_data[i].vertex.position.y << "  " << vertex_data[i].vertex.position.z << std::endl;
		}*/
	}
	_mesh.update_vertices();
}

void PlanetTile::set_parent_position(int x, int z, const glm::mat4 &transform) {
	float step = 1.0f / _resolution;
	float offset = 0.5f;
	glm::vec3 parent_position = glm::vec3(0, 0, 0);
	float cx = x * step - offset;
	float cz = z * step - offset;
	int counter = 0;

	int current_idx = (z + 1) + (x + 1)*(_resolution + 3);

	// If it's an even vertex then parent pos is same as the vertex pos
	if (!glm::mod(((float)(x + 1) + (z + 1)), 2.0f)) {
		parent_position = vertex_data[current_idx].vertex.position;
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

				glm::vec3 tmp_pos;
				tmp_pos = glm::vec3(transform *  glm::vec4(cx, 0, cz, 1.0));
				tmp_pos = glm::normalize(tmp_pos);
				float height = sampler.sample(tmp_pos);
				parent_position += (2500.0f + height * 0.1f) * tmp_pos;
			}
		}
	}

	vertex_data[current_idx].parent_position = parent_position / (float)counter;

	//std::cout << "parent: " << vertex_data[current_idx].parent_position.x << "  " << vertex_data[current_idx].parent_position.y << "  " << vertex_data[current_idx].parent_position.z << "  " << std::endl;
	//std::cout << "real: " << vertex_data[current_idx].own_position.x << "  " << vertex_data[current_idx].own_position.y << "  " << vertex_data[current_idx].own_position.z << "  " << std::endl;
	//Calculate vertex normals, simple version, only averages over one triangle

}


bool PlanetTile::is_edge(int x, int z) {
	return (x == -1) || (z == -1) || (x == _resolution + 1) || (z == _resolution + 1);
}

void PlanetTile::predraw(const Camera &camera) {
	if (_message_ref != -1) {
		auto message = MessageSystem::instance().get(_message_ref);
		if (message) {
			// Setup mesh once
			_mesh.setup_mesh();
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
	glm::mat4 model(1);
	//TODO: Evaluate if this works, could be used to simulate large distances?
	float distance = glm::distance(camera.get_eye(), glm::vec3(0, 0, 0));
	float fpc = 0.5 * camera.get_far();
	//model = glm::scale(glm::vec3(glm::exp(-distance / fpc)));
	//model = glm::translate(model, glm::vec3(glm::exp(-distance / fpc)));
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(_shader->program, "mvp"), 1, GL_FALSE, glm::value_ptr(camera.get_perspective() * camera.get_view() * model));
}

void PlanetTile::draw(const Camera & camera, double delta_time) {
	if (!_setup_done) { return; }
	predraw(camera);
	_mesh.draw(_shader, delta_time);
}

void PlanetTile::draw_wireframe(const Camera & camera, double delta_time) {
	if (!_setup_done) { return; }
	predraw(camera);
	_mesh.draw_wireframe(_shader, delta_time);
}
