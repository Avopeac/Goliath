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
	void Setup(float fovy, float aspect, float zNear, float zFar);
	void Update(glm::vec3 &eye, glm::vec3 &look, glm::vec3 &up); //Called every frame

	//Intersection methods
	int PointInFrustum(const glm::vec3 &point);
	int SphereInFrustum(const Sphere<float> &sphere);
	int BoxInFrustum(const AABB<float> &aabb);

	//Status for intersections
	enum { OUTSIDE = 0, INSIDE, INTERSECT };

	void DebugDraw(Shader &shader);

	// Plane array indices
	enum { TOP = 0, BOTTOM, LEFT, RIGHT, FAR, NEAR };
	// Point array indices
	enum { NTL = 0, NTR, NBL, NBR, FTL, FTR, FBL, FBR };

	const Plane<float> &GetPlane(int plane) const { if (plane > 0 && plane < 6) { return planes[plane]; } }
	const glm::vec3 &GetPoint(int point) const { if (point > 0 && point < 8) { return points[point]; } }

protected:
		
	float fovy;
	float aspect;
	float zNear;
	float zFar;

	float nearWidth;
	float nearHeight;
	float farWidth;
	float farHeight;
	
	Mesh frustumMesh;

	Plane<float> planes[FRUSTUM_PLANES];
	glm::vec3 points[FRUSTUM_POINTS];
};