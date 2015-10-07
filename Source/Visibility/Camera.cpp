#include "Camera.h"
#include <GLM\gtx\rotate_vector.hpp>

Camera::Camera() : Camera(0, 0, 0) {}
Camera::Camera(double x, double y, double z) : Camera(glm::dvec3(x, y, z)) {}
Camera::Camera(glm::dvec3 position) : eye(position), rotation(1) {}
Camera::~Camera() {}

void Camera::SetYaw(double degrees) { yaw = degrees; }
void Camera::SetPitch(double degrees) { pitch = degrees; }
void Camera::SetRoll(double degrees) { roll = degrees; }
void Camera::SetPosition(glm::dvec3 position) { eye = position; }
void Camera::SetPosition(double x, double y, double z) { eye = glm::dvec3(x, y, z); }

void Camera::SetPerspective(double fovy, double aspect, double zNear, double zFar) {
	this->fovx = glm::degrees(glm::atan(glm::tan(glm::radians(fovy) * 0.5) * aspect) * 2.0);
	this->fovy = fovy;
	this->aspect = aspect;
	
	near = zNear;
	far = zFar;

	projection = glm::perspective(fovy, aspect, zNear, zFar);
	cameraFrustum.Setup(fovy, aspect, zNear, zFar);
}

void Camera::SetPerspective(double fovy, double width, double height, double zNear, double zFar) {
	SetPerspective(fovy, width / height, zNear, zFar);
}

void Camera::SetOrthographic(double left, double right, double bottom, double top) {
	projection = glm::ortho(left, right, bottom, top);
}

void Camera::Update(float deltaTime) {

	Controller &c = Controller::GetInstance();
	float dx = c.GetMouseDelta().x * deltaTime;// *directionSpeed;
	float dy = c.GetMouseDelta().y * deltaTime;// *directionSpeed;

	yaw += dx;
	pitch += dy;

	//Determine actions
	glm::dvec3 deltaPosition(0);
	unsigned int count = c.GetCount();
	bool changed = false;
	for (unsigned int i = 0; i < count; ++i) {

		KeyAction::ActionType t = c.PeekAction();
		if (t == KeyAction::ActionType::MOVE_FORWARD) {
			deltaPosition -= look; changed = true;
		}
		if (t == KeyAction::ActionType::MOVE_BACK) {
			deltaPosition += look; changed = true;
		}
		if (t == KeyAction::ActionType::MOVE_LEFT) {
			deltaPosition += right; changed = true;
		}
		if (t == KeyAction::ActionType::MOVE_RIGHT) {
			deltaPosition -= right; changed = true;
		}
		if (t == KeyAction::ActionType::MOVE_UP) {
			deltaPosition -= up; changed = true;
		}
		if (t == KeyAction::ActionType::MOVE_DOWN) {
			deltaPosition += up; changed = true;
		}
		if (t == KeyAction::ActionType::INCREASE_SPEED) {
			translationSpeed += 1000; changed = true;
		}
		if (t == KeyAction::ActionType::DECREASE_SPEED) {
			translationSpeed -= 1000; changed = true;
		}

		std::cout << translationSpeed << std::endl;

		if (changed) {
			c.ConsumeAction();
		}
	}
	eye -= deltaPosition * translationSpeed * (double)deltaTime;

	//TODO: Something is not right here, movement stops in right direction when maximum pitch is reached
	if (pitch > MAX_Y_ROTATION_DEGREES) { pitch = MAX_Y_ROTATION_DEGREES; }
	else if (pitch < MIN_Y_ROTATION_DEGREES) { pitch = MIN_Y_ROTATION_DEGREES; }
	look = glm::highp_dvec3(glm::sin(glm::radians(-yaw)) * glm::cos(glm::radians(pitch)),
		glm::sin(glm::radians(pitch)),
		glm::cos(glm::radians(-yaw)) * glm::cos(glm::radians(pitch)));
	look = glm::normalize(look);
	right = cross(look, up);
	view = lookAt(eye, eye + look, up);
	cameraFrustum.Update(eye, eye + look, up);
}