#pragma once
#include "..\Model\Mesh.h"
#include "..\Model\Shader.h"
#include "..\Math\Plane.h"
#include "..\Math\AABB.h"
#include "..\Math\Sphere.h"
#include <GLM\glm.hpp>


#define FRUSTUM_PLANES 6
#define FRUSTUM_POINTS 8

/// The frustum defines the six planes that contains the camera's view. 
/// This is very useful when utilizing many forms of visible surface detection.
class Frustum {

public:
	Frustum() = default;
	~Frustum() = default;

	//Send camera parameters
	void Setup(double fovy, double aspect, double zNear, double zFar);
	void Update(glm::highp_dvec3 &eye, glm::highp_dvec3 &look, glm::highp_dvec3 &up); //Called every frame

	//Intersection methods
	int PointInFrustum (const glm::highp_dvec3 &point) const;
	int SphereInFrustum (const Sphere<double> &sphere) const;
	int BoxInFrustum (const AABB<double> &aabb) const;

	//Status for intersections
	enum { OUTSIDE = 0, INSIDE, INTERSECT };

	void DebugDraw(Shader &shader);

	// Plane array indices
	enum { TOP = 0, BOTTOM, LEFT, RIGHT, FAR, NEAR };
	// Point array indices
	enum { NTL = 0, NTR, NBL, NBR, FTL, FTR, FBL, FBR };

	const Plane<double> &GetPlane(int plane) const { if (plane > 0 && plane < 6) { return planes[plane]; } }
	const glm::highp_dvec3 &GetPoint(int point) const { if (point > 0 && point < 8) { return points[point]; } }

protected:
		
	double fovy;
	double aspect;
	double zNear;
	double zFar;

	double nearWidth;
	double nearHeight;
	double farWidth;
	double farHeight;
	
	Mesh frustumMesh;

	Plane<double> planes[FRUSTUM_PLANES];
	glm::highp_dvec3 points[FRUSTUM_POINTS];
};