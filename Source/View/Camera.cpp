#include "Camera.h"
#include <iostream>
#include <AntTweakBar/AntTweakBar.h>
#include "Input/Input.h"

Camera::Camera(const glm::dvec3 &eye, const glm::dvec3 &center, const glm::dvec3 &world_up, double vertical_fov, double aspect_ratio, double near, double far)
	: InputEnabled(), _vertical_fov(vertical_fov), _aspect_ratio(aspect_ratio), _near(near), _far(far), _eye(eye) {
	_projection = glm::perspective(_vertical_fov, _aspect_ratio, _near, _far);
	_horizontal_fov = glm::degrees(glm::atan(glm::tan(glm::radians(_vertical_fov) * 0.5) * _aspect_ratio) * 2.0);

	//Camera origin
	_base_forward = glm::normalize(center - eye);
	_base_right = glm::cross(_base_forward, glm::normalize(world_up));
	_base_up = glm::cross(_base_right, _base_forward);

	TwAddSeparator(Input::_tw_bar, "Camera", nullptr);
	TwAddVarRW(Input::_tw_bar, "Camera Speed", TW_TYPE_DOUBLE, &_movement_speed, " min=1.00 max=20000000.0 step=100.0 ");
}

void Camera::update(double delta_time) {
	//Apply the individual axis rotations
	//_next_rotation_quat = glm::angleAxis((float)_yaw, _base_up);
	//_next_rotation_quat = glm::rotate(_next_rotation_quat, (float)_pitch, _base_right);
	_next_rotation_quat = glm::angleAxis(_roll, _base_forward);
	_next_rotation_quat = glm::rotate(_next_rotation_quat, _yaw, _base_up);
	_next_rotation_quat = glm::rotate(_next_rotation_quat, _pitch, _base_right);

	//TODO: Apply roll somewhere
	//The scale on third parameter to SLERP determines speed of interpolation
	_rotation_quat = glm::slerp(_rotation_quat, _next_rotation_quat, delta_time * 5.0);
	_next_forward = glm::rotate(_rotation_quat, _base_forward);
	_next_right = glm::rotate(_rotation_quat, _base_right);
	_next_up = glm::rotate(_rotation_quat, _base_up);
	//Multiply rotation and translation matrices to get view matrix, keeping to quaternions to avoid Gimbal locking
	_view = glm::lookAt(_eye, _eye + _next_forward, _next_up);
	_view_proj = _projection * _view;

	if (_update_frustum) { build_frustum(); }

	_time += delta_time;
}

void Camera::handle_mouse_movement(double x, double y, double delta_x, double delta_y, double delta_time, bool captured) {
	if (captured) {
		_yaw += delta_x * 0.0001;
		_pitch += delta_y * 0.0001;
	}
}

void Camera::handle_multiple_keystrokes(GLFWwindow *window, double delta_time) {
	//Create a translation in the original orthonormal basis
	if (glfwGetKey(window, GLFW_KEY_W)) { _eye += glm::rotate(_rotation_quat, _base_forward) * delta_time * _movement_speed; }
	if (glfwGetKey(window, GLFW_KEY_S)) { _eye -= glm::rotate(_rotation_quat, _base_forward) * delta_time * _movement_speed; }
	if (glfwGetKey(window, GLFW_KEY_A)) { _eye -= glm::rotate(_rotation_quat, _base_right) * delta_time * _movement_speed; }
	if (glfwGetKey(window, GLFW_KEY_D)) { _eye += glm::rotate(_rotation_quat, _base_right) * delta_time * _movement_speed; }

	//For rolling
	if (glfwGetKey(window, GLFW_KEY_Q)) { _roll += delta_time; }
	if (glfwGetKey(window, GLFW_KEY_E)) { _roll -= delta_time; }
}

void Camera::handle_key_inputs(int key, int scan_code, int action, int mods, double delta_time) {
	if (key == GLFW_KEY_F && action == GLFW_PRESS) { _update_frustum = false; }
	if (key == GLFW_KEY_G && action == GLFW_PRESS) { _update_frustum = true; }
}

void Camera::build_frustum() {
	//Find frustum dimensions
	double near_height = 2.0 * glm::tan(glm::radians(_vertical_fov) * 0.5) * _near;
	double near_width = near_height * _aspect_ratio;
	double far_height = 2.0 * glm::tan(glm::radians(_vertical_fov) * 0.5) * _far;
	double far_width = far_height * _aspect_ratio;

	glm::dvec3 near_center = _eye + _next_forward * _near;
	glm::dvec3 far_center = _eye + _next_forward * _far;

	_points[0] = near_center - _next_up * near_height - _next_right * near_width; //nbl
	_points[1] = near_center + _next_up * near_height - _next_right * near_width; //ntl
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
}




