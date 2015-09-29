#pragma once
#include <GLM\glm.hpp>
struct Vertex {
	Vertex(glm::vec3 positionHigh = { 0, 0, 0 }, glm::vec3 positionLow = { 0, 0, 0 }, glm::vec3 normal = { 0, 0, 0 }, glm::vec2 texcoord = { 0, 0 })
		: positionHigh(positionHigh), positionLow(positionLow), normal(normal), texcoord(texcoord) {}

	glm::vec3 positionHigh;
	glm::vec3 positionLow;
	glm::vec3 normal;
	glm::vec2 texcoord;	
};
