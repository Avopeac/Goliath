#include "Camera.h"
#include <GLM\gtx\rotate_vector.hpp>

Camera::Camera(double x, double y, double z) : Camera(glm::dvec3(x, y, z)) {}
Camera::Camera(glm::dvec3 position) : _eye(position), _rotation(1) {}
Camera::~Camera() {}

void Camera::set_yaw(double degrees) { _yaw = degrees; }
void Camera::set_pitch(double degrees) { _pitch = degrees; }
void Camera::set_roll(double degrees) { _roll = degrees; }
void Camera::set_position(glm::dvec3 position) { _eye = position; }
void Camera::set_position(double x, double y, double z) { _eye = glm::dvec3(x, y, z); }

void Camera::set_perspective(double fovy, double aspect, double zNear, double zFar) {
	//Compute the horizontal field of view
	_fov_x = glm::degrees(glm::atan(glm::tan(glm::radians(fovy) * 0.5) * aspect) * 2.0);
	_fov_y = fovy;
	_aspect = aspect;
	_near = zNear;
	_far = zFar;
	_projection = glm::perspective(fovy, aspect, zNear, zFar);
	_camera_frustum.setup(fovy, aspect, zNear, zFar);
}

void Camera::set_perspective(double fovy, double width, double height, double zNear, double zFar) {
	set_perspective(fovy, width / height, zNear, zFar);
}

void Camera::set_orthographic(double left, double right, double bottom, double top) {
	_projection = glm::ortho(left, right, bottom, top);
}

void Camera::update(float deltaTime) {
	//Get the controller and check the mouse and keyboard for input
	Controller &c = Controller::instance();
	float dx = c.get_mouse_delta().x * deltaTime;// *directionSpeed;
	float dy = c.get_mouse_delta().y * deltaTime;// *directionSpeed;
	_yaw += dx;
	_pitch += dy;
	//Determine actions
	glm::dvec3 delta_position(0);
	unsigned int count = c.get_count();
	bool changed = false;
	for (unsigned int i = 0; i < count; ++i) {
		//Lots of actions for the camera
		KeyAction::ActionType t = c.peek_action();
		if (t == KeyAction::ActionType::MOVE_FORWARD) {
			delta_position -= _look; changed = true;
		}
		if (t == KeyAction::ActionType::MOVE_BACK) {
			delta_position += _look; changed = true;
		}
		if (t == KeyAction::ActionType::MOVE_LEFT) {
			delta_position += _right; changed = true;
		}
		if (t == KeyAction::ActionType::MOVE_RIGHT) {
			delta_position -= _right; changed = true;
		}
		if (t == KeyAction::ActionType::MOVE_UP) {
			delta_position -= _up; changed = true;
		}
		if (t == KeyAction::ActionType::MOVE_DOWN) {
			delta_position += _up; changed = true;
		}
		if (t == KeyAction::ActionType::INCREASE_SPEED) {
			_translation_speed += 1000; changed = true;
		}
		if (t == KeyAction::ActionType::DECREASE_SPEED) {
			_translation_speed -= 1000; changed = true;
		}
		
		//If any action was used, then pop it from the stack
		if (changed) {
			c.consume_action();
		}
	}
	//Propagate camera with the changes from the input
	_eye -= delta_position * _translation_speed * (double)deltaTime;
	//TODO: Something is not right here, movement stops in right direction when maximum pitch is reached
	if (_pitch > MAX_Y_ROTATION_DEGREES) { _pitch = MAX_Y_ROTATION_DEGREES; }
	else if (_pitch < MIN_Y_ROTATION_DEGREES) { _pitch = MIN_Y_ROTATION_DEGREES; }
	//Convert cartesian values to polar values
	_look = glm::highp_dvec3(glm::sin(glm::radians(-_yaw)) * glm::cos(glm::radians(_pitch)),
		glm::sin(glm::radians(_pitch)),
		glm::cos(glm::radians(-_yaw)) * glm::cos(glm::radians(_pitch)));
	//Set up camera orientation vectors
	_look = glm::normalize(_look);
	_right = cross(_look, _up);
	_view = lookAt(_eye, _eye + _look, _up);
	//Send orientation to camera frustum
	_camera_frustum.update(_eye, _eye + _look, _up);
}