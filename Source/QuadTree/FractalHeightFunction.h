#pragma once
#include "HeightFunction.h"
///A fractal height function adds gradient noise with weights and returns something that resembles a fractal behaviour, this looks nice for terrain surfaces.
class FractalHeightFunction : public HeightFunction {
public:
	//The fractal dimension, how jagged will the terrain be?
	const double dimension = 0.25;
	//The lacunarity which describes how similar it is
	const double lacunarity = 2.0;
	//The octaves which gives more detail as we close in
	const double octaves = 8.0;
	//The offset for the weights
	const double offset = 0.7;

	FractalHeightFunction() = default;
	~FractalHeightFunction() override = default;

	// Inherited via HeightFunction, uses simplex noise with a 3 dimensional point as the seed
	double get_value(glm::highp_dvec3 point) override;
};
