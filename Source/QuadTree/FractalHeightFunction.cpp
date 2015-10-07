#include "FractalHeightFunction.h"
#include <GLM\gtc\noise.hpp>

double FractalHeightFunction::get_value(glm::highp_dvec3 point) {
	return 1;// glm::simplex(point / 10000.0) * 8848.0;
}
