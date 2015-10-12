#pragma once
#include "HeightFunction.h"
///A fractal height function adds gradient noise with weights and returns something that resembles a fractal behaviour, this looks nice for terrain surfaces.
class FractalHeightFunction : public HeightFunction {
public:
	//The fractal dimension, how jagged will the terrain be?
	const double _dimension;
	//The lacunarity which describes how similar it is
	const double _lacunarity;
	//The octaves which gives more detail as we close in
	const double _octaves;
	//The offset for the weights, the heterogeneity of the terrain
	const double _offset;

	FractalHeightFunction(double dimension = 0.6, double lacunarity = 2.0, double octaves = 14.0, double offset = 0.0)
		: _dimension(dimension), _lacunarity(lacunarity), _octaves(octaves), _offset(offset) {}
	~FractalHeightFunction() override = default;

	// Inherited via HeightFunction, uses simplex noise with a 3 dimensional point as the seed
	double get_value(glm::highp_dvec3 point) override;
};
