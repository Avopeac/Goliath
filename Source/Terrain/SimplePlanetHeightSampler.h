#pragma once
#include <vector>
#include "Noise3D.h"
#include "PlanetHeightSampler.h"
class SimplePlanetHeightSampler : public PlanetHeightSampler {
public:
	SimplePlanetHeightSampler(double lacunarity, double octaves, double dimensionality, double offset)
		: PlanetHeightSampler(), _offset(offset){
		set_lacunarity(lacunarity);
		set_dimensionality(dimensionality);
		set_octaves(octaves);
		for (int i = 0; i <= _octaves; ++i) {
			_exponentials.push_back(glm::pow(_lacunarity, -_dimensionality * i));
		}
	}
	double sample(const glm::dvec3 &position) override {
		double i, v = 1.0;
		glm::dvec3 p(position);
		v += (glm::perlin(p) + _offset);
		p *= _lacunarity;
		for (i = 1.0; i <= _octaves; ++i) {
			v += (glm::abs(Noise3D::get_noise(p.x, p.y, p.z) + _offset) * glm::pow(_lacunarity, -_dimensionality * i));
			p *= _lacunarity;
		}
		return pow(v, 3.0) - 1.5;
	}

private:
	std::vector<double> _exponentials;
	double _offset;
};
