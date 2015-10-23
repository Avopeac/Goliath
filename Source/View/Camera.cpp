#include "Camera.h"
#include <iostream>

Camera::Camera(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up, double vertical_fov, double aspect_ratio, double near, double far)
	: InputEnabled(), _eye(eye), _center(center), _up(up), _vertical_fov(vertical_fov), _aspect_ratio(aspect_ratio), _near(near), _far(far) {
	//Create orthonormal basis
	_world_up = glm::normalize(_up);
	_direction = glm::normalize(_center - _eye);
	_right = glm::normalize(glm::cross(_world_up, _direction));
	_up = glm::cross(_direction, _right);
	//Create orientation quaternion
	glm::mat3 m(-_right, _up, -_direction);
	_orientation = glm::quat_cast(m);
	//Create perspective matrix
	_perspective = glm::perspective(_vertical_fov, _aspect_ratio, _near, _far);
}

void Camera::update(double delta_time) {
	glm::quat q = glm::conjugate(_orientation);
	_view = glm::mat4_cast(q);
	_view = glm::translate(_view, -_eye);
}

bool Camera::intersects_point(const glm::vec3 &point) {
	return false;
}

bool Camera::intersects_sphere(const glm::vec3 &center, double radius) {
	return false;
}

bool Camera::intersects_box(const glm::vec3 &center, const glm::vec3 &extents) {
	return false;
}

void Camera::handle_mouse_movement(double x, double y, double delta_x, double delta_y, double acc_x, double acc_y, double delta_time) {
	double rx = delta_x * delta_time * 0.005f;
	double ry = delta_y * delta_time * 0.005f;
	set_yaw(rx);
	set_pitch(-ry);
}

void Camera::set_pitch(double amount) {
	glm::quat q = glm::angleAxis((float)amount, _right);
	_up = glm::rotate(q, _up);
	_direction = glm::rotate(q, _direction);
	_orientation = q * _orientation;
}

void Camera::set_yaw(double amount) {
	glm::quat q = glm::angleAxis((float)amount, _up);
	_right = glm::rotate(q, _right);
	_direction = glm::rotate(q, _direction);
	_orientation = q * _orientation;
}

void Camera::set_roll(double amount) {
	glm::quat q = glm::angleAxis((float)amount, _direction);
	_up = glm::rotate(q, _up);
	_right = glm::rotate(q, _right);
	_orientation = q * _orientation;
}

void Camera::handle_multiple_keystrokes(GLFWwindow *window, double delta_time) {
	float delta = (float)delta_time;
	if (glfwGetKey(window, GLFW_KEY_W)) {
		_eye += _direction * delta;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		_eye -= _direction * delta;
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		_eye -= _right * delta;
		_direction += _eye;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		_eye += _right * delta;
		_direction += _eye;
	}
	if (glfwGetKey(window, GLFW_KEY_Q)) {
		set_roll(-delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_E)) {
		set_roll(delta_time);
	}
}




