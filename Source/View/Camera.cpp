#include "Camera.h"
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtc\quaternion.hpp>
#include <iostream>


Camera::Camera(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up, double vertical_fov, double aspect_ratio, double near, double far) : InputEnabled(), _eye(eye), _center(center), _up(up), _vertical_fov(vertical_fov), _near(near), _far(far) {
	//std::cout << "FORWARD: " << _forward.x << " " << _forward.y << " " << _forward.z << std::endl;
	//std::cout << "RIGHT: " << _right.x << " " << _right.y << " " << _right.z << std::endl;
	//std::cout << "UP: " << _up.x << " " << _up.y << " " << _up.z << std::endl;
	_view = glm::lookAt(_eye, _center, _up);
	_perspective = glm::perspective(vertical_fov, aspect_ratio, near, far);
}

void Camera::update(double delta_time) {
	//_view = glm::lookAt(_eye, _center, _up);
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
	/*double rx = acc_x * delta_time;
	double ry = acc_y * delta_time;

	std::cout << "Rx: " << rx << " Ry: " << ry << std::endl;
	glm::vec3 axis = glm::normalize(glm::cross(_center - _eye, _up));
	glm::quat quat(_view);
	glm::rotate(quat, (float)rx, axis);
	glm::rotate(quat, (float)ry, _up);
	_center.x = quat.x;
	_center.y = quat.y;
	_center.z = quat.z;*/
}

void Camera::handle_key_inputs(int key, int scan_code, int action, int mods, double delta_time) {

}


