#pragma once
#include "Primitive.h"
#include <GLM\glm.hpp>

/// A plane primitive consists of its plane equation. Its first 3 parameters, namely A, B and C is the normal of the plane.
/// The parameter D corresponds to the offset of the plane with respect to origo.
/// Distances from a point in space to the plane can be calculated to determine which half-space the point resides in.
template <typename T> class Plane : public Primitive<T> {

public:

	Plane(const glm::vec4 &plane) : a(plane.x), b(plane.y), c(plane.z), d(plane.w) {};
	Plane(const glm::vec3 &point, const glm::vec3 &normal) : a(normal.x), b(normal.y), c(normal.z) { d = -(a * point.x + b * point.y + c * point.z); }
	Plane(const glm::vec3 &point1, const glm::vec3 &point2, const glm::vec3 &point3) : Plane<T>(point1, cross(point2 - point1, point3 - point1)) {}

	Plane() = default;
	~Plane() override = default;

	float GetDistanceToPoint(const glm::vec3 &point) const {
		return abs(a * point.x + b * point.y + c * point.z + d);
	}

	float GetSignedDistanceToPoint(const glm::vec3 &point) const {
		return a * point.x + b * point.y + c * point.z + d;
	}

	void SetA(const T &a) { this->a = a; }
	void SetB(const T &b) { this->b = b; }
	void SetC(const T &c) { this->c = c; }
	void SetD(const T &d) { this->d = d; }

	const T &GetA() const { return a; }
	const T &GetB() const { return b; }
	const T &GetC() const { return c; }
	const T &GetD() const { return d; }

	T ComputeArea() override { return static_cast<T>(0.0); }
	T ComputeVolume() override { return static_cast<T>(0.0); }

private:

	T a;
	T b;
	T c;
	T d;
};