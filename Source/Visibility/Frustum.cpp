#include <iostream>
#include <vector>
#include <GLM\gtc\matrix_access.hpp>

#include "Frustum.h"

void Frustum::Update(glm::vec3 &eye, glm::vec3 &look, glm::vec3 &up) {

	glm::vec3 z = glm::normalize(look - eye);
	glm::vec3 x = glm::normalize(glm::cross(z, up));
	glm::vec3 y = glm::normalize(glm::cross(x, z));

	glm::vec3 nearCenter = eye + z * zNear;
	glm::vec3 farCenter = eye + z * zFar;

	points[NTL] = nearCenter + y * nearHeight - x * nearWidth;
	points[NTR] = nearCenter + y * nearHeight + x * nearWidth;
	points[NBL] = nearCenter - y * nearHeight - x * nearWidth;
	points[NBR] = nearCenter - y * nearHeight + x * nearWidth;

	points[FTL] = farCenter + y * farHeight - x * farWidth;
	points[FTR] = farCenter + y * farHeight + x * farWidth;
	points[FBL] = farCenter - y * farHeight - x * farWidth;
	points[FBR] = farCenter - y * farHeight + x * farWidth;

	planes[TOP] = Plane<float>(points[NTR], points[NTL], points[FTL]);
	planes[BOTTOM] = Plane<float>(points[NBL], points[NBR], points[FBR]);
	planes[LEFT] = Plane<float>(points[NTL], points[NBL], points[FBL]);
	planes[RIGHT] = Plane<float>(points[NBR], points[NTR], points[FBR]);
	planes[NEAR] = Plane<float>(points[NTL], points[NTR], points[NBR]);
	planes[FAR] = Plane<float>(points[FTR], points[FTL], points[FBL]);
}

void Frustum::DebugDraw(Shader &shader) {

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	vertices.push_back(points[FBL]);
	vertices.push_back(points[FBR]);
	vertices.push_back(points[FTR]);
	vertices.push_back(points[FTL]);

	vertices.push_back(points[NBL]);
	vertices.push_back(points[NBR]);
	vertices.push_back(points[NTR]);
	vertices.push_back(points[NTL]);

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

	glDisable(GL_CULL_FACE);
	frustumMesh.DrawWireframe(shader);
	glEnable(GL_CULL_FACE);
}

int Frustum::PointInFrustum(const glm::vec3 &point) {
	int status = INSIDE;
	for (unsigned short i = 0; i < FRUSTUM_PLANES; ++i) {
		if (planes[i].GetSignedDistanceToPoint(point) < 0.0) {
			return OUTSIDE;
		}
	}

	return status;
}

int Frustum::SphereInFrustum(const Sphere<float> &sphere) {
	int status = INSIDE;
	const glm::vec3 &c = sphere.GetCenter();
	const float &r = sphere.GetRadius();
	for (unsigned short i = 0; i < FRUSTUM_PLANES; ++i) {
		if (planes[i].GetSignedDistanceToPoint(c) - r < 0.0) {
			return OUTSIDE;
		}
	}

	return status;
}

int Frustum::BoxInFrustum(const AABB<float> &aabb) {
	const glm::vec3 &c = aabb.GetCenter();
	const float &o = aabb.GetExtent();

	// check box outside/inside of frustum
	for (int i = 0; i<6; i++) {
		int out = 0;

		out += planes[i].GetSignedDistanceToPoint(glm::vec3(c.x + o, c.y + o, c.z + o)) < 0.0 ? 1 : 0;
		out += planes[i].GetSignedDistanceToPoint(glm::vec3(c.x - o, c.y + o, c.z + o)) < 0.0 ? 1 : 0;
		out += planes[i].GetSignedDistanceToPoint(glm::vec3(c.x + o, c.y - o, c.z + o)) < 0.0 ? 1 : 0;
		out += planes[i].GetSignedDistanceToPoint(glm::vec3(c.x - o, c.y - o, c.z + o)) < 0.0 ? 1 : 0;
		out += planes[i].GetSignedDistanceToPoint(glm::vec3(c.x + o, c.y + o, c.z - o)) < 0.0 ? 1 : 0;
		out += planes[i].GetSignedDistanceToPoint(glm::vec3(c.x - o, c.y + o, c.z - o)) < 0.0 ? 1 : 0;
		out += planes[i].GetSignedDistanceToPoint(glm::vec3(c.x + o, c.y - o, c.z - o)) < 0.0 ? 1 : 0;
		out += planes[i].GetSignedDistanceToPoint(glm::vec3(c.x - o, c.y - o, c.z - o)) < 0.0 ? 1 : 0;

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

void Frustum::Setup(float fovy, float aspect, float zNear, float zFar) {
	this->fovy = fovy;
	this->aspect = aspect;
	this->zNear = zNear;
	this->zFar = zFar;

	nearHeight = 2.0f * glm::tan(glm::radians(fovy) * 0.5f) * zNear;
	nearWidth = nearHeight * aspect;

	farHeight = 2.0f * glm::tan(glm::radians(fovy) * 0.5f) * zFar;
	farWidth = farHeight * aspect;
}