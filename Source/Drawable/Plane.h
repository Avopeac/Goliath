#pragma once
#include <GLM\glm.hpp>
#include "Primitive.h"
//Simple mathematical plane class
class Plane : public Primitive {
public:
	Plane(double x = 0, double y = 0, double z = 0, double d = 0) : x(x), y(y), z(z), d(d) {};
	Plane(const glm::vec3 &normal, const glm::vec3 &point);
	Plane(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3);
	double distance(const glm::vec3 &point) const;
	double x, y, z, d;
};
