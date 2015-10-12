#pragma once
#include "Primitive.h"
#include <GLM\glm.hpp>
/// A plane primitive consists of its plane equation. Its first 3 parameters, namely A, B and C is the normal of the plane.
/// The parameter D corresponds to the offset of the plane with respect to origo.
/// Distances from a point in space to the plane can be calculated to determine which half-space the point resides in.
template <typename T> class Plane : public Primitive<T> {
public:
	Plane(const glm::vec4 &plane) : _a(plane.x), _b(plane.y), _c(plane.z), _d(plane.w) {};
	Plane(const glm::vec3 &point, const glm::vec3 &normal) : _a(normal.x), _b(normal.y), _c(normal.z) { _d = -(_a * point.x + _b * point.y + _c * point.z); }
	Plane(const glm::vec3 &point1, const glm::vec3 &point2, const glm::vec3 &point3) : Plane<T>(point1, cross(point2 - point1, point3 - point1)) {}
	Plane() = default;
	~Plane() override = default;

	//Get the absolute distance from plane to point, no side in regard
	float get_distance_to_point(const glm::vec3 &point) const {
		return abs(_a * point.x + _b * point.y + _c * point.z + _d);
	}

	//Get a signed distance from the plane to the given point, denotes which side of the plane point is on
	float get_signed_distance_to_point(const glm::vec3 &point) const {
		return _a * point.x + _b * point.y + _c * point.z + _d;
	}

	//Getters and setters for plane equation
	void set_a(const T &a) { _a = a; }
	void set_b(const T &b) { _b = b; }
	void set_c(const T &c) { _c = c; }
	void set_d(const T &d) { _d = d; }
	const T &get_a() const { return _a; }
	const T &get_b() const { return _b; }
	const T &get_c() const { return _c; }
	const T &get_d() const { return _d; }

	T compute_area() override { return static_cast<T>(0.0); }
	T compute_volume() override { return static_cast<T>(0.0); }

private:

	T _a;
	T _b;
	T _c;
	T _d;
};