#include "Plane.h"

Plane::Plane(const glm::dvec3 &normal, const glm::dvec3 &point) {
	x = normal.x;
	y = normal.y;
	z = normal.z;
	d = -glm::dot(normal, point);
}

Plane::Plane(const glm::dvec3 &p1, const glm::dvec3 &p2, const glm::dvec3 &p3) {
	glm::dvec3 v1 = p2 - p1;
	glm::dvec3 v2 = p3 - p1;
	glm::dvec3 normal = glm::normalize(glm::cross(v1, v2));
	x = normal.x;
	y = normal.y;
	z = normal.z;
	d = -glm::dot(normal, p1);
}

double Plane::distance(const glm::dvec3 &point) const {
	return x * point.x + y * point.y + z * point.z + d;
}

