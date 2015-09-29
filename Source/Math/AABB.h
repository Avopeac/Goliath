#pragma once
#include "Primitive.h"
#include <GLM\glm.hpp>

template <typename T> class AABB : public Primitive<T> {

public:

	AABB(const glm::vec3 &center, const T extent) : center(center), extent(extent) {};
	AABB(const AABB &aabb) = default;
	AABB() = default;
	~AABB() override = default;

	const glm::vec3 &GetCenter() const { return center; }
	const T &GetExtent() const { return extent; }
	
	T ComputeArea() override { return static_cast<T>(glm::pow((static_cast<T>(2) * extent), static_cast<T>(2))); }
	T ComputeVolume() override { return static_cast<T>(glm::pow((static_cast<T>(2) * extent), static_cast<T>(3))); }

	bool Intersects(const AABB<T> &a, const AABB<T> &b) {
		return (glm::abs(a.center.x - b.center.x) * 2 < (a.extent + b.extent)) &&
			(glm::abs(a.center.y - b.center.y) * 2 < (a.extent + b.extent)) &&
			(glm::abs(a.center.z - b.center.z) * 2 < (a.extent + b.extent));
	}

protected:

	glm::vec3 center;
	T extent;
};