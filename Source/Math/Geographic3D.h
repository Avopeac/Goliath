#pragma once
#include <iomanip>
#include "Type3D.h"
//How many decimals for printing
#ifndef GEOGRAPHIC3D_PRINT_PRECISION
#define GEOGRAPHIC3D_PRINT_PRECISION 10
#endif
///
/// Geographic3D is a representation of a three-component vector in a ellipsoidal coordinate system.
/// In our application it is very useful for mapping points from cartesian coordinates to a point on the ellipsoid.
///
class Geographic3D : public Type3D {
public:
	Geographic3D(double azimuth = 0.0, double inclination = 0.0, double radius = 0.0) : Type3D(azimuth, inclination, radius) {}
	Geographic3D(dvec_t v) : Type3D(v) {}
	~Geographic3D() override = default;
	
protected:
	// Inherited via Type3D
	void print(std::ostream & os) const override {
		os << std::setprecision(GEOGRAPHIC3D_PRINT_PRECISION)
			<< "[azimuth, inclination, radius] = " << "[" << _v.x << ", " << _v.y << ", " << _v.z << "]";
	}

};
