#pragma once
#include <GLM\glm.hpp>
/// A vertex is used for mesh and model objects, it also keeps vertex normal and texture coordinate
struct Vertex {
	Vertex(glm::vec3 position = { 0, 0 ,0 }, glm::vec3 normal = { 0, 0, 0 }, glm::vec2 texcoord = { 0, 0 })
		: position(position), normal(normal), texcoord(texcoord) {}
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};
