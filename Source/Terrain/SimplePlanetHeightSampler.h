#pragma once
#include <vector>
#include <iostream>
#include "Noise3D.h"
#include "PlanetHeightSampler.h"
class SimplePlanetHeightSampler : public PlanetHeightSampler {
public:
	SimplePlanetHeightSampler(double lacunarity, double octaves, double dimensionality, double offset)
		: PlanetHeightSampler(), _offset(offset) {
		set_lacunarity(lacunarity);
		set_dimensionality(dimensionality);
		set_octaves(octaves);


		//First run
		double frequency = 1.0;
		double scaler = 1.0;
		for (int i = 0; i < (int)_octaves; ++i) {
			_exponentials.push_back(glm::pow(frequency, -_dimensionality) * scaler);
			frequency *= lacunarity;
		}
		
	}

	/*
	* Some good parameter values to start with :
	* H : 0.25
	* offset : 0.7
	*/
	double sample(const glm::dvec3 &position) override {
		double frequency = 0.0, result = 0.0, signal = 0.0, weight = 0.0, remainder = 0.0;
		int i = 0;
		glm::dvec3 point(position);
		/* get first octave of function */
		result = (1.0 - glm::abs(Noise3D::get_noise(point.x, point.y, point.z))) * _exponentials[0];
		weight = result;
		/* increase frequency */
		point *= _lacunarity;
		/* spectral construction inner loop, where the fractal is built */
		for (i = 1; i < _octaves; i++) {
			/* prevent divergence */
			if (weight > 1.0) weight = 1.0;
			/* get next higher frequency */
			signal = ((1.0 - glm::abs(Noise3D::get_noise(point.x, point.y, point.z))) + _offset) * _exponentials[i];
			/* add it in, weighted by previous freq's local value */
			result += weight * signal;
			/* update the (monotonically decreasing) weighting value */
			/* (this is why H must specify a high fractal dimension) */
			weight *= signal;
			/* increase frequency */
			point *= _lacunarity;
		} /* for */
			/* take care of remainder in “octaves” */
		remainder = _octaves - (int)_octaves;
		if (remainder) {/* “i” and spatial freq. are preset in loop above */
			result += remainder * (1.0 - glm::abs(Noise3D::get_noise(point.x, point.y, point.z))) * _exponentials[i];
		}
		
		return result * 0.5 - 3.5;
	}

private:
	std::vector<double> _exponentials;
	double _offset;
};
