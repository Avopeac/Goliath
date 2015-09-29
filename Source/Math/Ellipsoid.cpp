#include "Ellipsoid.h"

const Cartesian3D Ellipsoid::EARTH = Cartesian3D(ELLIPSOID_EARTH_SEMI_MAJOR_METERS, ELLIPSOID_EARTH_SEMI_MAJOR_METERS, ELLIPSOID_EARTH_SEMI_MINOR_METERS);
const Cartesian3D Ellipsoid::UNIT = Cartesian3D(ELLIPSOID_UNIT_METERS, ELLIPSOID_UNIT_METERS, ELLIPSOID_UNIT_METERS);

Ellipsoid::Ellipsoid(Cartesian3D radii) : _radii(radii), _inverseRadii(1.0 / radii.get()), _squareRadii(radii.get() * radii.get()) {

}

Ellipsoid::Ellipsoid(double x, double y, double z) : Ellipsoid(Cartesian3D(x, y, z)) {

}

Cartesian3D Ellipsoid::geocentricSurfaceNormal(const Cartesian3D &point) {
	return glm::normalize(point.get());
}

Cartesian3D Ellipsoid::geodeticSurfaceNormal(const Cartesian3D &point) {
	return glm::normalize(point.get() * _inverseRadii.get());
}

Cartesian3D Ellipsoid::geodeticSurfaceNormal(const Geographic3D &geographic) {

	//Save for efficiency
	double cosPhi = glm::cos(geographic.get().y);

	//Mapping from spherical coordinates with height 1 to cartesian coordinates
	return Cartesian3D(
		cosPhi * glm::cos(geographic.get().x),
		cosPhi * glm::sin(geographic.get().x),
		glm::sin(geographic.get().y)
		);
}

Cartesian3D Ellipsoid::geographicToWgs84(const Geographic3D &geographic) {
	
	//Determine geodetic surface normal
	Cartesian3D n(geodeticSurfaceNormal(geographic));

	//Used in solving the equation for gamma
	Cartesian3D k(_squareRadii.get() * n.get());

	//Gamma is some scaling of the normal direction
	double gamma(glm::sqrt(glm::dot(k.get(), n.get())));

	//Find the surface point
	Cartesian3D surfacePoint(k.get() / gamma);

	//Offset by the height above the surface pointsss
	return surfacePoint.get() + (geographic.get().z * n.get());
}

Geographic3D Ellipsoid::wgs84SurfaceToGeographic(const Cartesian3D &point) {
	Cartesian3D n = geodeticSurfaceNormal(point);
	//Cartesian to geographic
	return Geographic3D(glm::atan(n.get().y, n.get().x), glm::asin(n.get().z / glm::length(n.get())));
}

Cartesian3D Ellipsoid::wgs84ToGeocentricSurface(const Cartesian3D &point) {
	double beta = 1.0 / glm::length(point.get() * _inverseRadii.get());
	//Scale point to surface along vector origo to point
	return beta * point.get();
}

Cartesian3D Ellipsoid::wgs84ToGeodeticSurface(const Cartesian3D &point) {
	//Implement...
	return Cartesian3D(0, 0, 0);
}


