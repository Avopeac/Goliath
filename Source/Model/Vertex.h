#pragma once
#include <GLM\glm.hpp>
/// A vertex is used for mesh and model objects, this vertex has two positional vectors to account for large distances, it also keeps vertex normal and texture coordinate
struct Vertex {
	Vertex(glm::vec3 position_high = { 0, 0, 0 }, glm::vec3 position_low = { 0, 0, 0 }, glm::vec3 normal = { 0, 0, 0 }, glm::vec2 texcoord = { 0, 0 })
		: _position_high(position_high), _position_low(position_low), _normal(normal), _texcoord(texcoord) {}
	glm::vec3 _position_high;
	glm::vec3 _position_low;
	glm::vec3 _normal;
	glm::vec2 _texcoord;	
};
