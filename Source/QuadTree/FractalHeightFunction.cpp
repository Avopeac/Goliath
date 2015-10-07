#include "FractalHeightFunction.h"
#include <GLM\gtc\noise.hpp>

double FractalHeightFunction::get_value(glm::highp_dvec3 point) {
	return glm::simplex(point / 100000.0) * 8848.0;
}
