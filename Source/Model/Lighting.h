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

	void setup_standard_lighting() {
		//Set up default lighting
		directions.push_back({ 0,1,0 }); //sun
		directions.push_back({ 0,1,0 }); //sky
		directions.push_back({ -1,0,-1 }); //indirect sun
		intensities.push_back({ 1.5, 1.5, 1.0 }); //sun color
		intensities.push_back({ 0.11, 0.161, 0.184 }); //sky color
		intensities.push_back({ 0.15, 0.15, 0.1 }); //indirect sun color
		lights = 3;
	}

	std::vector<glm::vec3> intensities;
	std::vector<glm::vec3> view_space_directions;
	std::vector<glm::vec3> directions;
	unsigned int get_max_num_lights() const { return max_lights; }
	unsigned int get_num_lights() const { return lights; }

private:
	unsigned int lights = 0;
	unsigned int max_lights = MAX_LIGHT_SOURCES;
};