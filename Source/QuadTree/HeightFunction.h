#pragma once
#include <GLM\glm.hpp>

class HeightFunction {
public:
	virtual ~HeightFunction() = default;
	virtual double get_value(glm::highp_dvec3 point) = 0;
protected:
	HeightFunction() = default;
};
