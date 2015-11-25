#pragma once
#include <GLM/glm.hpp>
#include "Primitive.h"
//Simple mathematical plane class
class Plane : public Primitive {
public:
	Plane(double x = 0, double y = 0, double z = 0, double d = 0) : x(x), y(y), z(z), d(d) {};
	Plane(const glm::dvec3 &normal, const glm::dvec3 &point);
	Plane(const glm::dvec3 &p1, const glm::dvec3 &p2, const glm::dvec3 &p3);
	double distance(const glm::dvec3 &point) const;
	double x, y, z, d;
};
