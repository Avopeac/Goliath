#pragma once
#include <vector>
#include <GLM\glm.hpp>
//Need to define a maximum amount of lights for GLSL
#ifndef MAX_LIGHT_SOURCES
#define MAX_LIGHT_SOURCES 20
#endif
struct Lighting {
	Lighting() = default;
	Lighting(const unsigned int lights, const std::vector<glm::vec3> &directions, const std::vector<glm::vec3> &intensities)
		: lights(lights), intensities(intensities), directions(directions), view_space_directions(directions) {}

	std::vector<glm::vec3> intensities;
	std::vector<glm::vec3> view_space_directions;
	std::vector<glm::vec3> directions;
	unsigned int get_max_num_lights() const { return max_lights; }
	unsigned int get_num_lights() const { return lights; }

private:
	unsigned int lights = 0;
	unsigned int max_lights = MAX_LIGHT_SOURCES;
};