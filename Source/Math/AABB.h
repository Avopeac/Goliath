#pragma once
#include <GLM\glm.hpp>
#include "Primitive.h"
///The axis aligned bounding box is used for the quadtree implementation in our planet renderer.
template <typename T> class AABB : public Primitive<T> {
public:
	AABB(const glm::highp_dvec3 &center, const T extent) : _center(center), _extent(extent) {};
	AABB(const AABB &aabb) = default;
	AABB() = default;
	~AABB() override = default;
	//Getters for bounding box
	const glm::highp_dvec3 &get_center() const { return _center; }
	const T &get_extent() const { return _extent; }
	//Metrics
	T compute_area() override { return static_cast<T>(glm::pow((static_cast<T>(2) * _extent), static_cast<T>(2))); }
	T compute_volume() override { return static_cast<T>(glm::pow((static_cast<T>(2) * _extent), static_cast<T>(3))); }
	//Check if one aabb intersects with another
	bool intersects(const AABB<T> &a, const AABB<T> &b) {
		return (glm::abs(a._center.x - b._center.x) * 2 < (a._extent + b._extent)) &&
			(glm::abs(a._center.y - b._center.y) * 2 < (a._extent + b._extent)) &&
			(glm::abs(a._center.z - b._center.z) * 2 < (a._extent + b._extent));
	}

protected:
	glm::highp_dvec3 _center;
	T _extent;
};