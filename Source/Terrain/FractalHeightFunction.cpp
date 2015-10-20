#include "FractalHeightFunction.h"
#include <GLM\gtc\noise.hpp>
#include <vector>

double FractalHeightFunction::get_value(glm::highp_dvec3 point) {
	//Loop counter
	int i;
	//All the values needed for the fractal noise
	double value, freq, result, signal, weight, remainder, restriction = 1.f, highest = 0.1f;
	//The first run of this algorithm will allocate the exponentials for each octave
	static bool first = true;
	static std::vector<double> exponentials(_octaves + 1);
	if (first) {
		freq = 1.0;
		for (i = 0; i < _octaves; ++i) {
			exponentials[i] = pow(freq, -_dimension);
			freq *= _lacunarity;
		}
		first = false;
	}
	//Save point in temporary for computation
	glm::highp_dvec3 temp = point;
	//Find the noise value for the first octave
	result = (glm::simplex(temp * restriction) + _offset) * exponentials.at(0);
	weight = result;
	temp.x *= _lacunarity;
	temp.y *= _lacunarity;
	temp.z *= _lacunarity;
	//For each remaining octave, accumulate noise
	for (i = 1; i < _octaves; ++i) {
		if (weight > 1.0) weight = 1.0;
		signal = (glm::simplex(temp * restriction) + _offset) * exponentials.at(i);
		result += weight * signal;
		temp.x *= _lacunarity;
		temp.y *= _lacunarity;
		temp.z *= _lacunarity;
	}
	//Add the remaider as height
	remainder = _octaves - (int)_octaves;
	if (remainder > 0.0) {
		result += remainder * glm::simplex(temp * restriction) * exponentials.at(i);
	}

	//Scale the result with the heighest point on the terrain, 8848 meters is the height of Mount Everest
	return result * highest;
}

