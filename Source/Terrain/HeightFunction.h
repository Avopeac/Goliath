#pragma once
#include <GLM\glm.hpp>
///The height function is an abstract base class that has a pure virtual method that can be implemented by some special height functions for the terrain.
class HeightFunction {
public:
	virtual ~HeightFunction() = default;
	//Get a double precision value with the point as the seed
	virtual double get_value(glm::highp_dvec3 point) = 0;
protected:
	HeightFunction() = default;
};
