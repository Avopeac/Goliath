#pragma once
#include <GLM\gtc\noise.hpp>
class PlanetHeightSampler {
public:
	virtual ~PlanetHeightSampler() {}
	virtual float sample(const glm::vec3 &position) = 0;
	virtual void set_lacunarity(float lacunarity) { _lacunarity = lacunarity; }
	virtual void set_dimensionality(float dimensionality) { _dimensionality = dimensionality; }
	virtual void set_octaves(float octaves) { _octaves = octaves; }
protected:
	PlanetHeightSampler() {}
	float _octaves = 0;
	float _lacunarity = 0;
	float _dimensionality = 0;
};
