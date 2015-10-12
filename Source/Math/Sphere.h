#pragma once
#include <GLM\glm.hpp>
#include "Primitive.h"
/// A sphere primitive is very useful to determine visible surfaces in our scenes.
/// Due to the fact that it's very easily described mathematically, intersections with spheres are computationally efficient.
template <typename T> class Sphere : public Primitive<T> {
public:
	Sphere(T x, T y, T z, T radius) : _center(x, y, z), _radius(radius) {}
	Sphere(glm::vec3 center, T radius) : _center(center), _radius(radius) {}
	Sphere() = default;
	~Sphere() override = default;
	//Getters and setters for sphere
	void set_center(glm::vec3 center) { _center = center; }
	void set_radius(T radius) { _radius = radius; }
	const glm::vec3 &get_center() const { return _center; }
	const T get_radius() const { return _radius; }
	//Inherited from Primitive
	T compute_area() override { return static_cast<T>(4.0 * 3.14159265359 * glm::pow(_radius, 2.0)); } 
	T compute_volume() override { return static_cast<T>(4.0 / 3.0 * 3.14159265359 * glm::pow(_radius, 3.0)); }

private:
	T _radius;
	glm::vec3 _center;
};