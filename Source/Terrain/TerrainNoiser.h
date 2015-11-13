#pragma once
#include <GLM\glm.hpp>
#include <GLM\gtc\noise.hpp>
class TerrainNoiser {
public:
	TerrainNoiser() {}
	double get_height(const glm::dvec3 &seed){
		return glm::simplex(seed);
	}
};