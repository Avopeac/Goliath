#pragma once
#include "Geographic3D.h"
#include "Cartesian3D.h"
#define ELLIPSOID_EARTH_SEMI_MAJOR_METERS 6378137.0
#define ELLIPSOID_EARTH_SEMI_MINOR_METERS 6356752.314256
#define ELLIPSOID_UNIT_METERS 1.0
///
/// Ellipsoid is defined with radii of three axes. It is useful in our application to approximate the shape of a planetary body.
/// An ellipsoid uses two main coordinate systems namely cartesian and geographical. An oblate ellipsoid has a semi-minor axis that is different from the semi-major ones.
/// Worth knowing is that the ellipsoidal representation of Earth is called WGS84.
/// Date: 2015-09-23
/// Author: Andreas Larsson
///
class Ellipsoid {

public:
	
	static const Cartesian3D EARTH;
	static const Cartesian3D UNIT;

	Ellipsoid(Cartesian3D radii = UNIT); 
	Ellipsoid(double x, double y, double z);

	const Cartesian3D &getRadii() const { return _radii; }
	const Cartesian3D &getInverseRadii() const { return _inverseRadii; }
	const Cartesian3D &getSquareRadii() const { return _squareRadii; }

	//The geodetic surface normal is the actual surface normal of an ellipsoid, maximum 45 degree difference from geocentric surface normal
	Cartesian3D geodeticSurfaceNormal(const Cartesian3D &point);
	Cartesian3D geodeticSurfaceNormal(const Geographic3D &geographic);

	//The geocentric surface normal is the normal in the direction from the point to the origin
	Cartesian3D geocentricSurfaceNormal(const Cartesian3D &point);
	
	//Convert a geographic coordinate to a cartesian coordinate
	Cartesian3D geographicToWgs84(const Geographic3D &geographic);

	//Convert point to geocentric surface point
	Cartesian3D wgs84ToGeocentricSurface(const Cartesian3D &point);

	//Convert point to geodetic surface point
	Cartesian3D wgs84ToGeodeticSurface(const Cartesian3D &point);

	//Convert point on surface to geographic coordinate
	Geographic3D wgs84SurfaceToGeographic(const Cartesian3D &point);

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


