#include "Plane.h"

Plane::Plane(const glm::vec3 & normal, const glm::vec3 & point) {
	x = (double)normal.x;
	y = (double)normal.y;
	z = (double)normal.z;
	d = -(double)glm::dot(normal, point);
}

Plane::Plane(const glm::vec3 & p1, const glm::vec3 & p2, const glm::vec3 & p3) {
	glm::vec3 v1 = p2 - p1;
	glm::vec3 v2 = p3 - p1;
	glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
	x = (double)normal.x;
	y = (double)normal.y;
	z = (double)normal.z;
	d = -(double)glm::dot(normal, p1);
}

double Plane::distance(const glm::vec3 &point) const {
	return x * point.x + y * point.y + z * point.z + d;
}

