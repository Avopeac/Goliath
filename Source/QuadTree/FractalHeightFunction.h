#pragma once
#include "HeightFunction.h"

class FractalHeightFunction : public HeightFunction {
public:
	FractalHeightFunction() = default;
	~FractalHeightFunction() override = default;

	// Inherited via HeightFunction
	double get_value(glm::highp_dvec3 point) override;
};
