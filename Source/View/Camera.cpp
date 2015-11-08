#include "Camera.h"
#include <iostream>

Camera::Camera(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &world_up, double vertical_fov, double aspect_ratio, double near, double far)
	: InputEnabled(), _vertical_fov(vertical_fov), _aspect_ratio(aspect_ratio), _near(near), _far(far), _eye(eye) {
	_perspective = glm::perspective(_vertical_fov, _aspect_ratio, _near, _far);
	//Camera origin
	_base_forward = glm::normalize(center - eye);
	_base_right = glm::cross(_base_forward, glm::normalize(world_up));
	_base_up = glm::cross(_base_right, _base_forward);
}

void Camera::update(double delta_time) {
	//Apply the individual axis rotations
	_next_rotation_quat = glm::angleAxis((float)_yaw, _base_up);
	_next_rotation_quat = glm::rotate(_next_rotation_quat, (float)_pitch, _base_right);
	//TODO: Apply roll somewhere
	//The scale on third parameter to SLERP determines speed of interpolation
	_rotation_quat = glm::slerp(_rotation_quat, _next_rotation_quat, (float)delta_time * 5.0f);
	_next_forward = glm::rotate(_rotation_quat, _base_forward);
	_next_right = glm::rotate(_rotation_quat, _base_right);
	_next_up = glm::rotate(_rotation_quat, _base_up);
	//Multiply rotation and translation matrices to get view matrix, keeping to quaternions to avoid Gimbal locking
	_view = glm::lookAt(_eye, _eye + _next_forward, _next_up);

	if (_update_frustum) {
		build_frustum();
	}

	_time += delta_time;
}

bool Camera::intersects_point(const glm::vec3 &point) const {
	for (unsigned short i = 0; i < 6; ++i) {
		Plane p(_frustum[i]);
		glm::vec3 n(p.x, p.y, p.z);
		if (glm::dot(n, point) + p.d > 0.0f) {
			return false;
		}
	}

	return true;
}

bool Camera::intersects_sphere(const glm::vec3 &center, double radius) const {
	for (unsigned short i = 0; i < 6; ++i) {
		Plane p(_frustum[i]);
		glm::vec3 n(p.x, p.y, p.z);
		if (glm::dot(n, center) + p.d > radius) {
			return false;
		}
	}

	return true;
}

bool Camera::intersects_box(const glm::vec3 &center, const glm::vec3 &extents) const {
	for (unsigned short i = 0; i < 6; ++i) {
		unsigned short outside = 0;
		Plane p(_frustum[i]);
		glm::vec3 n(p.x, p.y, p.z);
		outside += glm::dot(n, glm::vec3(center.x + extents.x, center.y + extents.y, center.z + extents.z)) + p.d > 0.0f ? 1 : 0;
		outside += glm::dot(n, glm::vec3(center.x + extents.x, center.y + extents.y, center.z - extents.z)) + p.d > 0.0f ? 1 : 0;
		outside += glm::dot(n, glm::vec3(center.x + extents.x, center.y - extents.y, center.z + extents.z)) + p.d > 0.0f ? 1 : 0;
		outside += glm::dot(n, glm::vec3(center.x + extents.x, center.y - extents.y, center.z - extents.z)) + p.d > 0.0f ? 1 : 0;
		outside += glm::dot(n, glm::vec3(center.x - extents.x, center.y + extents.y, center.z + extents.z)) + p.d > 0.0f ? 1 : 0;
		outside += glm::dot(n, glm::vec3(center.x - extents.x, center.y + extents.y, center.z - extents.z)) + p.d > 0.0f ? 1 : 0;
		outside += glm::dot(n, glm::vec3(center.x - extents.x, center.y - extents.y, center.z + extents.z)) + p.d > 0.0f ? 1 : 0;
		outside += glm::dot(n, glm::vec3(center.x - extents.x, center.y - extents.y, center.z - extents.z)) + p.d > 0.0f ? 1 : 0;
		if (outside == 8) return false;
	}
	return true;
}

void Camera::handle_mouse_movement(double x, double y, double delta_x, double delta_y, double delta_time, bool captured) {
	if (captured) {
		_yaw += delta_x * 0.0001;
		_pitch += delta_y * 0.0001;
	}
}

void Camera::handle_multiple_keystrokes(GLFWwindow *window, double delta_time) {
	//Create a translation in the original orthonormal basis
	if (glfwGetKey(window, GLFW_KEY_W)) {
		_eye += glm::rotate(_rotation_quat, _base_forward) * (float)delta_time * (float)_movement_speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		_eye -= glm::rotate(_rotation_quat, _base_forward) * (float)delta_time * (float)_movement_speed;
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		_eye -= glm::rotate(_rotation_quat, _base_right) * (float)delta_time * (float)_movement_speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		_eye += glm::rotate(_rotation_quat, _base_right) * (float)delta_time * (float)_movement_speed;
	}

	//For rolling
	if (glfwGetKey(window, GLFW_KEY_Q)) {
		_roll += delta_time;
	}
	if (glfwGetKey(window, GLFW_KEY_E)) {
		_roll -= delta_time;
	}
}

void Camera::handle_key_inputs(int key, int scan_code, int action, int mods, double delta_time) {
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		_update_frustum = false;
	}

	if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		_update_frustum = true;
	}
}

void Camera::build_frustum() {

	vertices.clear();
	indices.clear();

	float near_height = 2.0f * glm::tan(glm::radians((float)_vertical_fov) * 0.5f) * (float)_near;
	float near_width = near_height * (float)_aspect_ratio;
	float far_height = 2.0f * glm::tan(glm::radians((float)_vertical_fov) * 0.5f) * (float)_far;
	float far_width = far_height * (float)_aspect_ratio;

	glm::vec3 near_center = _eye + _next_forward * (float)_near;
	glm::vec3 far_center = _eye + _next_forward * (float)_far;

	_points[0] = near_center - _next_up * near_height - _next_right * near_width; //nbl
	_points[1] = near_center + _next_up * near_height - _next_right* near_width; //ntl
	_points[2] = near_center + _next_up * near_height + _next_right * near_width; //ntr
	_points[3] = near_center - _next_up * near_height + _next_right * near_width; //nbr

	_points[4] = far_center - _next_up * far_height - _next_right * far_width; //fbl
	_points[5] = far_center + _next_up * far_height - _next_right * far_width; //ftl
	_points[6] = far_center + _next_up * far_height + _next_right * far_width; //ftr
	_points[7] = far_center - _next_up * far_height + _next_right * far_width; //fbr

	_frustum[0] = Plane(_points[2], _points[1], _points[5]); //top
	_frustum[1] = Plane(_points[0], _points[3], _points[7]); //bot
	_frustum[2] = Plane(_points[1], _points[0], _points[4]); //left
	_frustum[3] = Plane(_points[3], _points[2], _points[7]); //right
	_frustum[4] = Plane(_points[1], _points[2], _points[3]); //near
	_frustum[5] = Plane(_points[6], _points[5], _points[4]); //far

	vertices.push_back(_points[4]); //fbl 1
	vertices.push_back(_points[7]); //fbr 2
	vertices.push_back(_points[6]); //ftr 3 
	vertices.push_back(_points[5]); //ftl 4
	vertices.push_back(near_center); //0

	indices.push_back(2); indices.push_back(4); indices.push_back(3); //top
	indices.push_back(4); indices.push_back(2); indices.push_back(1); //right
	indices.push_back(0); indices.push_back(4); indices.push_back(1); //bot
	indices.push_back(4); indices.push_back(0); indices.push_back(3); //left
}




