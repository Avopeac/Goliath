#pragma once
#include <iomanip>
#include "Type3D.h"
//How many digits when printing cartesian coordinates
#ifndef CARTESIAN3D_PRINT_PRECISION
#define CARTESIAN3D_PRINT_PRECISION 4
#endif
///
/// Cartesian3D is a representation of a three-component vector in a cartesian coordinate system.
/// The graphics libraries work with cartesian coordinates.
///
class Cartesian3D : public Type3D {
public:
	Cartesian3D(double x = 0.0, double y = 0.0, double z = 0.0) : Type3D(x, y, z) {}
	Cartesian3D(dvec_t v) : Type3D(v) {}
	~Cartesian3D() override = default;

protected:
	// Inherited via Type3D
	void print(std::ostream & os) const override {
		os << std::setprecision(CARTESIAN3D_PRINT_PRECISION)
			<< "[x, y, z] = " << "[" << _v.x << ", " << _v.y << ", " << _v.z << "]";
	}
};

