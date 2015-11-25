#pragma once
#include <GLM/glm.hpp>
//An object can derive this pure abstract base class, primitives often have intersection methods and may always have an origin
class Primitive {
public:
	virtual ~Primitive() {};
	virtual const glm::dvec3 &get_origin() const { return _origin; }
protected:
	Primitive(const glm::dvec3 &origin = { 0, 0, 0 }) : _origin(origin) {}
	glm::dvec3 _origin;
};