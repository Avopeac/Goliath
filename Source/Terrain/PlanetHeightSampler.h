#pragma once
#include <GLM\gtc\noise.hpp>
class PlanetHeightSampler {
public:
	virtual ~PlanetHeightSampler() {}
	virtual double sample(const glm::dvec3 &position) = 0;
	virtual void set_lacunarity(double lacunarity) { _lacunarity = lacunarity; }
	virtual void set_dimensionality(double dimensionality) { _dimensionality = dimensionality; }
	virtual void set_octaves(double octaves) { _octaves = octaves; }
protected:
	PlanetHeightSampler() {}
	double _octaves = 0;
	double _lacunarity = 0;
	double _dimensionality = 0;
};
