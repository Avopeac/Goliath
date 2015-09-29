#pragma once
#include "Primitive.h"
#include <GLM\glm.hpp>

/// A sphere primitive is very useful to determine visible surfaces in our scenes.
/// Due to the fact that it's very easily described mathematically, intersections with spheres are computationally efficient.
template <typename T> class Sphere : public Primitive<T> {

public:

	Sphere(T x, T y, T z, T radius) : center(x, y, z), radius(radius) {}
	Sphere(glm::vec3 center, T radius) : center(center), radius(radius) {}
	Sphere() = default;
	~Sphere() override = default;

	void SetCenter(glm::vec3 center) { this->center = center; }
	void SetRadius(T radius) { this->radius = radius; }
	const glm::vec3 &GetCenter() const { return center; }
	const T GetRadius() const { return radius; }

	T ComputeArea() override { return static_cast<T>(4.0 * 3.14159265359 * glm::pow(radius, 2.0)); } 
	T ComputeVolume() override { return static_cast<T>(4.0 / 3.0 * 3.14159265359 * glm::pow(radius, 3.0)); }

private:

	T radius;
	glm::vec3 center;
};