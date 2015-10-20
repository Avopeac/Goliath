#include <iostream>
#include <vector>
#include "..\Math\MathHelp.h"
#include "Frustum.h"

void Frustum::update(glm::highp_dvec3 &eye, glm::highp_dvec3 &look, glm::highp_dvec3 &up) {
	//Create a orthonormal basis, important to not that the up vector may be different from calculations in camera
	glm::highp_dvec3 z = glm::normalize(look - eye);
	glm::highp_dvec3 x = glm::normalize(glm::cross(z, up));
	glm::highp_dvec3 y = glm::normalize(glm::cross(x, z));
	//Get the center of the view planes
	glm::highp_dvec3 near_center = eye + z * _z_near;
	glm::highp_dvec3 far_center = eye + z * _z_far;
	//Find near points
	_points[NEAR_TOP_LEFT] = near_center + y * _near_height - x * _near_width;
	_points[NEAR_TOP_RIGHT] = near_center + y * _near_height + x * _near_width;
	_points[NEAR_BOTTOM_LEFT] = near_center - y * _near_height - x * _near_width;
	_points[NEAR_BOTTOM_RIGHT] = near_center - y * _near_height + x * _near_width;
	//Find far points
	_points[FAR_TOP_LEFT] = far_center + y * _far_height - x * _far_width;
	_points[FAR_TOP_RIGHT] = far_center + y * _far_height + x * _far_width;
	_points[FAR_BOTTOM_LEFT] = far_center - y * _far_height - x * _far_width;
	_points[FAR_BOTTOM_RIGHT] = far_center - y * _far_height + x * _far_width;
	//Set planes with the found points
	_planes[TOP_PLANE] = Plane<double>(_points[NEAR_TOP_RIGHT], _points[NEAR_TOP_LEFT], _points[FAR_TOP_LEFT]);
	_planes[BOTTOM_PLANE] = Plane<double>(_points[NEAR_BOTTOM_LEFT], _points[NEAR_BOTTOM_RIGHT], _points[FAR_BOTTOM_RIGHT]);
	_planes[LEFT_PLANE] = Plane<double>(_points[NEAR_TOP_LEFT], _points[NEAR_BOTTOM_LEFT], _points[FAR_BOTTOM_LEFT]);
	_planes[RIGHT_PLANE] = Plane<double>(_points[NEAR_BOTTOM_RIGHT], _points[NEAR_TOP_RIGHT], _points[FAR_BOTTOM_RIGHT]);
	_planes[NEAR_PLANE] = Plane<double>(_points[NEAR_TOP_LEFT], _points[NEAR_TOP_RIGHT], _points[NEAR_BOTTOM_RIGHT]);
	_planes[FAR_PLANE] = Plane<double>(_points[FAR_TOP_RIGHT], _points[FAR_TOP_LEFT], _points[FAR_BOTTOM_LEFT]);
}

void Frustum::convert_vertex(Vertex &vertex, const glm::highp_dvec3 &point) {
	float xp, xl, yp, yl, zp, zl;
	MathHelp::double_to_two_floats(point.x, xp, xl);
	MathHelp::double_to_two_floats(point.y, yp, yl);
	MathHelp::double_to_two_floats(point.z, zp, zl);
	vertex._position_high = glm::highp_vec3(xp, yp, zp);
	vertex._position_low = glm::highp_vec3(xl, yl, zl);
}

void Frustum::debug_draw(Shader &shader) {
	//The mesh data for debugging
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	//Convert a double to 2 floating point numbers
	Vertex v1,v2,v3,v4,v5,v6,v7,v8;
	convert_vertex(v1, _points[FAR_BOTTOM_LEFT]);
	convert_vertex(v2, _points[FAR_BOTTOM_RIGHT]);
	convert_vertex(v3, _points[FAR_TOP_RIGHT]);
	convert_vertex(v4, _points[FAR_TOP_LEFT]);
	convert_vertex(v5, _points[NEAR_BOTTOM_LEFT]);
	convert_vertex(v6, _points[NEAR_BOTTOM_RIGHT]);
	convert_vertex(v7, _points[NEAR_TOP_RIGHT]);
	convert_vertex(v8, _points[NEAR_TOP_LEFT]);
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v4);
	vertices.push_back(v5);
	vertices.push_back(v6);
	vertices.push_back(v7);
	vertices.push_back(v8);
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
	//Setup frustum mesh
	_frustum_mesh.vertices = vertices;
	_frustum_mesh.indices = indices;
	_frustum_mesh.setup_mesh();
	//Disable culling for the debug draw
	glDisable(GL_CULL_FACE);
	_frustum_mesh.draw_wireframe(shader);
	glEnable(GL_CULL_FACE);
}

int Frustum::point_in_frustum(const glm::highp_dvec3 &point) const {
	int status = INSIDE;
	for (unsigned short i = 0; i < FRUSTUM_PLANES; ++i) {
		if (_planes[i].get_signed_distance_to_point(point) < 0.0) {
			return OUTSIDE;
		}
	}
	return status;
}

int Frustum::sphere_in_frustum(const Sphere<double> &sphere) const {
	int status = INSIDE;
	const glm::highp_dvec3 &c = sphere.get_center();
	const double &r = sphere.get_radius();
	for (unsigned short i = 0; i < FRUSTUM_PLANES; ++i) {
		if (_planes[i].get_signed_distance_to_point(c) - r < 0.0) {
			return OUTSIDE;
		}
	}
	return status;
}

int Frustum::box_in_frustum(const AABB<double> &aabb) const {
	const glm::highp_dvec3 &center = aabb.get_center();
	const double &offset = aabb.get_extent();
	// check box outside/inside of frustum
	for (int i = 0; i < 6; i++) {
		int out = 0;
		out += _planes[i].get_signed_distance_to_point(glm::highp_dvec3(center.x + offset, center.y + offset, center.z + offset)) < 0.0 ? 1 : 0;
		out += _planes[i].get_signed_distance_to_point(glm::highp_dvec3(center.x - offset, center.y + offset, center.z + offset)) < 0.0 ? 1 : 0;
		out += _planes[i].get_signed_distance_to_point(glm::highp_dvec3(center.x + offset, center.y - offset, center.z + offset)) < 0.0 ? 1 : 0;
		out += _planes[i].get_signed_distance_to_point(glm::highp_dvec3(center.x - offset, center.y - offset, center.z + offset)) < 0.0 ? 1 : 0;
		out += _planes[i].get_signed_distance_to_point(glm::highp_dvec3(center.x + offset, center.y + offset, center.z - offset)) < 0.0 ? 1 : 0;
		out += _planes[i].get_signed_distance_to_point(glm::highp_dvec3(center.x - offset, center.y + offset, center.z - offset)) < 0.0 ? 1 : 0;
		out += _planes[i].get_signed_distance_to_point(glm::highp_dvec3(center.x + offset, center.y - offset, center.z - offset)) < 0.0 ? 1 : 0;
		out += _planes[i].get_signed_distance_to_point(glm::highp_dvec3(center.x - offset, center.y - offset, center.z - offset)) < 0.0 ? 1 : 0;
		if (out == 8) return OUTSIDE;
	}
	// check frustum outside/inside box
	int out;
	out = 0; for (int i = 0; i<8; i++) out += ((_points[i].x > center.x + offset) ? 1 : 0); if (out == 8) return OUTSIDE;
	out = 0; for (int i = 0; i<8; i++) out += ((_points[i].x < center.x - offset) ? 1 : 0); if (out == 8) return OUTSIDE;
	out = 0; for (int i = 0; i<8; i++) out += ((_points[i].y > center.y + offset) ? 1 : 0); if (out == 8) return OUTSIDE;
	out = 0; for (int i = 0; i<8; i++) out += ((_points[i].y < center.y - offset) ? 1 : 0); if (out == 8) return OUTSIDE;
	out = 0; for (int i = 0; i<8; i++) out += ((_points[i].z > center.z + offset) ? 1 : 0); if (out == 8) return OUTSIDE;
	out = 0; for (int i = 0; i<8; i++) out += ((_points[i].z < center.z - offset) ? 1 : 0); if (out == 8) return OUTSIDE;
	return INSIDE;
}

void Frustum::setup(double fov_y, double aspect, double z_near, double z_far) {
	_fov_y = fov_y;
	_aspect = aspect;
	_z_near = z_near;
	_z_far = z_far;
	_near_height = 2.0f * glm::tan(glm::radians(fov_y) * 0.5f) * z_near;
	_near_width = _near_height * aspect;
	_far_height = 2.0f * glm::tan(glm::radians(fov_y) * 0.5f) * z_far;
	_far_width = _far_height * aspect;
}