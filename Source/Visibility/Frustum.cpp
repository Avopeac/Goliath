#include <iostream>
#include <vector>
#include <GLM\gtc\matrix_access.hpp>

#include "..\Math\MathHelp.h"

#include "Frustum.h"

void Frustum::Update(glm::highp_dvec3 &eye, glm::highp_dvec3 &look, glm::highp_dvec3 &up) {

	glm::highp_dvec3 z = glm::normalize(look - eye);
	glm::highp_dvec3 x = glm::normalize(glm::cross(z, up));
	glm::highp_dvec3 y = glm::normalize(glm::cross(x, z));

	glm::highp_dvec3 nearCenter = eye + z * zNear;
	glm::highp_dvec3 farCenter = eye + z * zFar;

	points[NTL] = nearCenter + y * nearHeight - x * nearWidth;
	points[NTR] = nearCenter + y * nearHeight + x * nearWidth;
	points[NBL] = nearCenter - y * nearHeight - x * nearWidth;
	points[NBR] = nearCenter - y * nearHeight + x * nearWidth;

	points[FTL] = farCenter + y * farHeight - x * farWidth;
	points[FTR] = farCenter + y * farHeight + x * farWidth;
	points[FBL] = farCenter - y * farHeight - x * farWidth;
	points[FBR] = farCenter - y * farHeight + x * farWidth;

	planes[TOP] = Plane<double>(points[NTR], points[NTL], points[FTL]);
	planes[BOTTOM] = Plane<double>(points[NBL], points[NBR], points[FBR]);
	planes[LEFT] = Plane<double>(points[NTL], points[NBL], points[FBL]);
	planes[RIGHT] = Plane<double>(points[NBR], points[NTR], points[FBR]);
	planes[NEAR] = Plane<double>(points[NTL], points[NTR], points[NBR]);
	planes[FAR] = Plane<double>(points[FTR], points[FTL], points[FBL]);
}

void Frustum::DebugDraw(Shader &shader) {

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	float xp, xl, yp, yl, zp, zl;
	MathHelp::doubleToTwoFloats(points[FBL].x, xp, xl);
	MathHelp::doubleToTwoFloats(points[FBL].y, yp, yl);
	MathHelp::doubleToTwoFloats(points[FBL].z, zp, zl);

	Vertex v;
	v.positionHigh = glm::highp_vec3(xp, yp, zp);
	v.positionLow = glm::highp_vec3(xl, yl, zl);
	vertices.push_back(v);

	MathHelp::doubleToTwoFloats(points[FBR].x, xp, xl);
	MathHelp::doubleToTwoFloats(points[FBR].y, yp, yl);
	MathHelp::doubleToTwoFloats(points[FBR].z, zp, zl);

	v.positionHigh = glm::highp_vec3(xp, yp, zp);
	v.positionLow = glm::highp_vec3(xl, yl, zl);
	vertices.push_back(v);

	MathHelp::doubleToTwoFloats(points[FTR].x, xp, xl);
	MathHelp::doubleToTwoFloats(points[FTR].y, yp, yl);
	MathHelp::doubleToTwoFloats(points[FTR].z, zp, zl);

	v.positionHigh = glm::highp_vec3(xp, yp, zp);
	v.positionLow = glm::highp_vec3(xl, yl, zl);
	vertices.push_back(v);

	MathHelp::doubleToTwoFloats(points[FTL].x, xp, xl);
	MathHelp::doubleToTwoFloats(points[FTL].y, yp, yl);
	MathHelp::doubleToTwoFloats(points[FTL].z, zp, zl);

	v.positionHigh = glm::highp_vec3(xp, yp, zp);
	v.positionLow = glm::highp_vec3(xl, yl, zl);
	vertices.push_back(v);

	MathHelp::doubleToTwoFloats(points[NBL].x, xp, xl);
	MathHelp::doubleToTwoFloats(points[NBL].y, yp, yl);
	MathHelp::doubleToTwoFloats(points[NBL].z, zp, zl);

	v.positionHigh = glm::highp_vec3(xp, yp, zp);
	v.positionLow = glm::highp_vec3(xl, yl, zl);
	vertices.push_back(v);
	
	MathHelp::doubleToTwoFloats(points[NBR].x, xp, xl);
	MathHelp::doubleToTwoFloats(points[NBR].y, yp, yl);
	MathHelp::doubleToTwoFloats(points[NBR].z, zp, zl);

	v.positionHigh = glm::highp_vec3(xp, yp, zp);
	v.positionLow = glm::highp_vec3(xl, yl, zl);
	vertices.push_back(v);

	MathHelp::doubleToTwoFloats(points[NTR].x, xp, xl);
	MathHelp::doubleToTwoFloats(points[NTR].y, yp, yl);
	MathHelp::doubleToTwoFloats(points[NTR].z, zp, zl);

	v.positionHigh = glm::highp_vec3(xp, yp, zp);
	v.positionLow = glm::highp_vec3(xl, yl, zl);
	vertices.push_back(v);

	MathHelp::doubleToTwoFloats(points[NTL].x, xp, xl);
	MathHelp::doubleToTwoFloats(points[NTL].y, yp, yl);
	MathHelp::doubleToTwoFloats(points[NTL].z, zp, zl);

	v.positionHigh = glm::highp_vec3(xp, yp, zp);
	v.positionLow = glm::highp_vec3(xl, yl, zl);
	vertices.push_back(v);

	
	//Front
	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(2); indices.push_back(3); indices.push_back(0);
	//Top
	indices.push_back(3); indices.push_back(2); indices.push_back(6);
	indices.push_back(6); indices.push_back(7); indices.push_back(3);
	//Back
	indices.push_back(7); indices.push_back(6); indices.push_back(5);
	indices.push_back(5); indices.push_back(4); indices.push_back(7);
	//Bottom
	indices.push_back(4); indices.push_back(5); indices.push_back(1);
	indices.push_back(1); indices.push_back(0); indices.push_back(4);
	//Left
	indices.push_back(4); indices.push_back(0); indices.push_back(3);
	indices.push_back(3); indices.push_back(7); indices.push_back(4);
	//Right
	indices.push_back(1); indices.push_back(5); indices.push_back(6);
	indices.push_back(6); indices.push_back(2); indices.push_back(1);

	frustumMesh.vertices = vertices;
	frustumMesh.indices = indices;
	frustumMesh.SetupMesh();

	//glDisable(GL_CULL_FACE);
	frustumMesh.DrawWireframe(shader);
	//glEnable(GL_CULL_FACE);
}

int Frustum::PointInFrustum(const glm::highp_dvec3 &point) const {
	int status = INSIDE;
	for (unsigned short i = 0; i < FRUSTUM_PLANES; ++i) {
		if (planes[i].GetSignedDistanceToPoint(point) < 0.0) {
			return OUTSIDE;
		}
	}

	return status;
}

int Frustum::SphereInFrustum(const Sphere<double> &sphere) const {
	int status = INSIDE;
	const glm::highp_dvec3 &c = sphere.GetCenter();
	const double &r = sphere.GetRadius();
	for (unsigned short i = 0; i < FRUSTUM_PLANES; ++i) {
		if (planes[i].GetSignedDistanceToPoint(c) - r < 0.0) {
			return OUTSIDE;
		}
	}

	return status;
}

int Frustum::BoxInFrustum(const AABB<double> &aabb) const {
	const glm::highp_dvec3 &c = aabb.GetCenter();
	const double &o = aabb.GetExtent();

	// check box outside/inside of frustum
	for (int i = 0; i<6; i++) {
		int out = 0;

		out += planes[i].GetSignedDistanceToPoint(glm::highp_dvec3(c.x + o, c.y + o, c.z + o)) < 0.0 ? 1 : 0;
		out += planes[i].GetSignedDistanceToPoint(glm::highp_dvec3(c.x - o, c.y + o, c.z + o)) < 0.0 ? 1 : 0;
		out += planes[i].GetSignedDistanceToPoint(glm::highp_dvec3(c.x + o, c.y - o, c.z + o)) < 0.0 ? 1 : 0;
		out += planes[i].GetSignedDistanceToPoint(glm::highp_dvec3(c.x - o, c.y - o, c.z + o)) < 0.0 ? 1 : 0;
		out += planes[i].GetSignedDistanceToPoint(glm::highp_dvec3(c.x + o, c.y + o, c.z - o)) < 0.0 ? 1 : 0;
		out += planes[i].GetSignedDistanceToPoint(glm::highp_dvec3(c.x - o, c.y + o, c.z - o)) < 0.0 ? 1 : 0;
		out += planes[i].GetSignedDistanceToPoint(glm::highp_dvec3(c.x + o, c.y - o, c.z - o)) < 0.0 ? 1 : 0;
		out += planes[i].GetSignedDistanceToPoint(glm::highp_dvec3(c.x - o, c.y - o, c.z - o)) < 0.0 ? 1 : 0;

		if (out == 8) return OUTSIDE;
	}

	// check frustum outside/inside box
	int out;
	out = 0; for (int i = 0; i<8; i++) out += ((points[i].x > c.x + o) ? 1 : 0); if (out == 8) return OUTSIDE;
	out = 0; for (int i = 0; i<8; i++) out += ((points[i].x < c.x - o) ? 1 : 0); if (out == 8) return OUTSIDE;
	out = 0; for (int i = 0; i<8; i++) out += ((points[i].y > c.y + o) ? 1 : 0); if (out == 8) return OUTSIDE;
	out = 0; for (int i = 0; i<8; i++) out += ((points[i].y < c.y - o) ? 1 : 0); if (out == 8) return OUTSIDE;
	out = 0; for (int i = 0; i<8; i++) out += ((points[i].z > c.z + o) ? 1 : 0); if (out == 8) return OUTSIDE;
	out = 0; for (int i = 0; i<8; i++) out += ((points[i].z < c.z - o) ? 1 : 0); if (out == 8) return OUTSIDE;

	return INSIDE;
}

void Frustum::Setup(double fovy, double aspect, double zNear, double zFar) {
	this->fovy = fovy;
	this->aspect = aspect;
	this->zNear = zNear;
	this->zFar = zFar;

	nearHeight = 2.0f * glm::tan(glm::radians(fovy) * 0.5f) * zNear;
	nearWidth = nearHeight * aspect;

	farHeight = 2.0f * glm::tan(glm::radians(fovy) * 0.5f) * zFar;
	farWidth = farHeight * aspect;
}