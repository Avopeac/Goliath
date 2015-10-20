#pragma once
#include <GLM\glm.hpp>
#include "..\Model\Mesh.h"
#include "..\Model\Shader.h"
#include "..\Math\Plane.h"
#include "..\Math\AABB.h"
#include "..\Math\Sphere.h"
//The number of planes in a frustum
#ifndef FRUSTUM_PLANES
#define FRUSTUM_PLANES 6
#endif
//The number of points in a frustum
#ifndef FRUSTUM_POINTS
#define FRUSTUM_POINTS 8
#endif
/// The frustum defines the six planes that contains the camera's view. 
/// This is very useful when utilizing many forms of visible surface detection.
class Frustum {
public:
	Frustum() = default;
	//Send camera parameters
	void setup(double fov_y, double aspect, double z_near, double z_far);
	//Called every frame
	void update(glm::highp_dvec3 &eye, glm::highp_dvec3 &look, glm::highp_dvec3 &up); 
	//Intersection methods
	int point_in_frustum (const glm::highp_dvec3 &point) const;
	int sphere_in_frustum (const Sphere<double> &sphere) const;
	int box_in_frustum (const AABB<double> &aabb) const;
	//Status for intersections
	enum { OUTSIDE = 0, INSIDE, INTERSECT };
	//Costly debug drawing method
	void debug_draw(Shader &shader);
	//Plane array indices
	enum { TOP_PLANE = 0, BOTTOM_PLANE, LEFT_PLANE, RIGHT_PLANE, FAR_PLANE, NEAR_PLANE };
	//Point array indices
	enum { NEAR_TOP_LEFT = 0, NEAR_TOP_RIGHT, NEAR_BOTTOM_LEFT, NEAR_BOTTOM_RIGHT, FAR_TOP_LEFT, FAR_TOP_RIGHT, FAR_BOTTOM_LEFT, FAR_BOTTOM_RIGHT };
	//Get a given plane 
	const Plane<double> &get_plane(int plane) const { if (plane > 0 && plane < 6) { return _planes[plane]; } }
	//Get a given point
	const glm::highp_dvec3 &get_point(int point) const { if (point > 0 && point < 8) { return _points[point]; } }

protected:
	//Camera parameters
	double _fov_y;
	double _aspect;
	double _z_near;
	double _z_far;
	//View plane measures
	double _near_width;
	double _near_height;
	double _far_width;
	double _far_height;
	//Debug draw mesh
	Mesh _frustum_mesh;
	//Collection of the planes
	Plane<double> _planes[FRUSTUM_PLANES];
	//Collection of the points
	glm::highp_dvec3 _points[FRUSTUM_POINTS];
	//A friend function for converting double precision to float precision when debugging
	void convert_vertex(Vertex &vertex, const glm::highp_dvec3 &point);
};