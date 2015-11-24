#pragma once
#include <vector>
#include "Noise3D.h"
#include "PlanetHeightSampler.h"
class SimplePlanetHeightSampler : public PlanetHeightSampler {
public:
	SimplePlanetHeightSampler(float lacunarity, float octaves, float dimensionality, float offset) 
		: PlanetHeightSampler(), _offset(offset){
		set_lacunarity(lacunarity);
		set_dimensionality(dimensionality);
		set_octaves(octaves);
		for (int i = 0; i <= _octaves; ++i) {
			_exponentials.push_back(glm::pow(_lacunarity, -_dimensionality * i));
		}
	}
	float sample(const glm::vec3 &position) override {
		float i, v = 1.0f;
		glm::vec3 p(position);
		v += (glm::perlin(p) + _offset);
		p *= _lacunarity;
		for (i = 1.0f; i <= _octaves; ++i) {
			v += (glm::abs(Noise3D::get_noise(p.x, p.y, p.z) + _offset) * glm::pow(_lacunarity, -_dimensionality * i));
			p *= _lacunarity;
		}
		return pow(v, 3.0f) - 1.5f;
	}

private:
	std::vector<float> _exponentials;
	float _offset;
};
