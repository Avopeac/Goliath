#pragma once
#include "Geographic3D.h"
#include "Cartesian3D.h"
//The earth's horizontal size
#ifndef ELLIPSOID_EARTH_SEMI_MAJOR_METERS
#define ELLIPSOID_EARTH_SEMI_MAJOR_METERS 6378137.0
#endif
//The earth's vertical size
#ifndef ELLIPSOID_EARTH_SEMI_MINOR_METERS
#define ELLIPSOID_EARTH_SEMI_MINOR_METERS 6356752.314256
#endif
//The unit sphere
#ifndef ELLIPSOID_UNIT_METERS
#define ELLIPSOID_UNIT_METERS 1.0
#endif
///
/// Ellipsoid is defined with radii of three axes. It is useful in our application to approximate the shape of a planetary body.
/// An ellipsoid uses two main coordinate systems namely cartesian and geographical. An oblate ellipsoid has a semi-minor axis that is different from the semi-major ones.
/// Worth knowing is that the ellipsoidal representation of Earth is called WGS84.
///
class Ellipsoid {
public:
	static const Cartesian3D _earth;
	static const Cartesian3D _unit;
	Ellipsoid(Cartesian3D radii = _unit); 
	Ellipsoid(double x, double y, double z);
	//Getters for ellipsoid parameters
	const Cartesian3D &getRadii() const { return _radii; }
	const Cartesian3D &getInverseRadii() const { return _inverseRadii; }
	const Cartesian3D &getSquareRadii() const { return _squareRadii; }
	//The geodetic surface normal is the actual surface normal of an ellipsoid, maximum 45 degree difference from geocentric surface normal
	Cartesian3D geodetic_surface_normal(const Cartesian3D &point);
	Cartesian3D geodeticSurfaceNormal(const Geographic3D &geographic);
	//The geocentric surface normal is the normal in the direction from the point to the origin
	Cartesian3D geocentric_surface_normal(const Cartesian3D &point);
	//Convert a geographic coordinate to a cartesian coordinate
	Cartesian3D geographic_to_wgs84(const Geographic3D &geographic);
	//Convert point to geocentric surface point
	Cartesian3D wgs84_to_geocentric_surface(const Cartesian3D &point);
	//Convert point to geodetic surface point
	Cartesian3D wgs84_to_geodetic_surface(const Cartesian3D &point);
	//Convert point on surface to geographic coordinate
	Geographic3D wgs84_surface_to_geographic(const Cartesian3D &point);
	friend std::ostream &operator<<(std::ostream &os, const Ellipsoid &ellipsoid) {
		os << "[Radii] = " << ellipsoid._radii << std::endl;
		os << "[Inverse Radii] = " << ellipsoid._inverseRadii << std::endl;
		os << "[Squared Radii] = " << ellipsoid._squareRadii << std::endl;
		return os;
	}
	
private:
	Cartesian3D _radii;
	Cartesian3D _inverseRadii;
	Cartesian3D _squareRadii;
	Cartesian3D _inverseSquareRadii;
};


