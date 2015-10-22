#pragma once
#include <GLM\glm.hpp>
class Primitive {
public:
	virtual ~Primitive() {};
	virtual const glm::vec3 &get_origin() const { return _origin; }
protected:
	Primitive(const glm::vec3 &origin = { 0, 0, 0 }) : _origin(origin) {}
	glm::vec3 _origin;
};