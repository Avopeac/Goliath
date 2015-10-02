#include "FractalHeightFunction.h"
#include <GLM\gtc\noise.hpp>

double FractalHeightFunction::get_value(glm::highp_dvec3 point) {
	return glm::simplex(point / 10000.0) * 1000.0;
}
