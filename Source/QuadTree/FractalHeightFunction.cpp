#include "FractalHeightFunction.h"
#include <GLM\gtc\noise.hpp>
#include <vector>

double FractalHeightFunction::get_value(glm::highp_dvec3 point) {
	//Loop counter
	int i;
	//All the values needed for the fractal noise
	double value, freq, result, signal, weight, remainder, restriction = 0.0000005, highest = 8848.0;
	//The first run of this algorithm will allocate the exponentials for each octave
	static bool first = true;
	static std::vector<double> exponentials(octaves + 1);
	if (first) {
		freq = 1.0;
		for (i = 0; i < octaves; ++i) {
			exponentials[i] = pow(freq, -dimension);
			freq *= lacunarity;
		}
		first = false;
	}
	//Save point in temporary for computation
	glm::highp_dvec3 temp = point;
	//Find the noise value for the first octave
	result = (glm::simplex(temp * restriction) + offset) * exponentials.at(0);
	weight = result;
	temp.x *= lacunarity;
	temp.y *= lacunarity;
	temp.z *= lacunarity;
	//For each remaining octave, accumulate noise
	for (i = 1; i < octaves; ++i) {
		if (weight > 1.0) weight = 1.0;
		signal = (glm::simplex(temp * restriction) + offset) * exponentials.at(i);
		result += weight * signal;
		temp.x *= lacunarity;
		temp.y *= lacunarity;
		temp.z *= lacunarity;
	}
	//Add the remaider as height
	remainder = octaves - (int)octaves;
	if (remainder > 0.0) {
		result += remainder * glm::simplex(temp * restriction) * exponentials.at(i);
	}
	//Make sharper peaks when result is less than zero
	if (result < 0.0) {
		result = glm::abs(result);
	}
	//Scale the result with the heighest point on the terrain, 8848 meters is the height of Mount Everest
	return result * highest;
}

