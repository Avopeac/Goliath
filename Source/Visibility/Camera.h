#pragma once
#include "..\Application.h"
#include "..\Controller\Controller.h"
#include "Frustum.h"
#include <iostream>
#include <GLM\glm.hpp>
#include <GLM\mat4x4.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtx\transform.hpp>

#define MAX_Y_ROTATION_DEGREES 89.999f
#define MIN_Y_ROTATION_DEGREES -89.999f

/// A camera is what views the scene. 
/// We don't actually move the camera in the world. We move to world in front of the camera. 
/// This is a very helpful thing to keep in mind when doing transformations.
class Camera {

public:
	Camera();
	Camera(double x, double y, double z);
	Camera(glm::dvec3 position);
	~Camera();

	void SetYaw(double degrees);
	void SetPitch(double degrees);
	void SetRoll(double degrees);
	void SetPosition(glm::dvec3 position);
	void SetPosition(double x, double y, double z);

	void SetOrthographic(double left, double right, double bottom, double top);
	void SetPerspective(double fovy, double aspect, double zNear, double zFar);
	void SetPerspective(double fovy, double width, double height, double zNear, double zFar);

	void Update(float deltaTime);

	const double GetFovy() const { return fovy; }
	const double GetAspect() const { return aspect; }
	const double GetNear() const { return near; }
	const double GetFar() const { return far; }

	const glm::dvec3 &GetEyeVector() const { return eye; }
	const glm::dvec3 &GetLookVector() const { return look; }
	const glm::dvec3 &GetUpVector() const { return up; }
	const glm::dvec3 &GetRightVector() const { return right; }

	const double GetYaw() const { return yaw; }
	const double GetPitch() const { return pitch; }
	const double GetRoll() const { return roll; }

	const glm::dmat4 &GetProjectionMatrix() const { return projection; }
	const glm::dmat4 &GetViewMatrix() const { return view; }

	const Frustum &GetFrustum() const { return cameraFrustum; }

protected:

	Frustum cameraFrustum;

	glm::dmat4 rotation;
	glm::dmat4 projection;
	glm::dmat4 view;

	double fovy = .0;
	double aspect = .0;
	double near = .0;
	double far = .0;

	glm::dvec3 eye = glm::dvec3(0, 0, 0);
	glm::dvec3 look = glm::dvec3(0, 0, -1);
	glm::dvec3 right = glm::dvec3(1, 0, 0);
	glm::dvec3 up = glm::dvec3(0, 1, 0);

	double yaw = .0;
	double pitch = .0;
	double roll = .0;

	double translationSpeed = 999999.0;
	double directionSpeed = 0.5;

};

inline std::ostream& operator<<(std::ostream &os, glm::vec3 vector) {
	os << "[" << vector.x << ", " << vector.y << ", " << vector.z << "]";
	return os;
}

inline std::ostream& operator<<(std::ostream &os, Camera &camera) {
	os << "FIELD::OF::VIEW::" << camera.GetFovy() << '\n';
	os << "ASPECT::RATIO::" << camera.GetAspect() << '\n';
	os << "NEAR::PLANE::DIST::" << camera.GetNear() << '\n';
	os << "FAR::PLANE::DIST::" << camera.GetFar() << '\n';
	os << "PITCH::ANGLE::" << camera.GetPitch() << '\n';
	os << "YAW::ANGLE::" << camera.GetYaw() << '\n';
	os << "ROLL::ANGLE::" << camera.GetRoll() << '\n';
	os << "EYE::POSITION::VECTOR::" << -camera.GetEyeVector() << '\n';
	os << "LOOK::POSITION::VECTOR::" << camera.GetLookVector() << '\n';
	os << "UP::DIRECTION::VECTOR::" << camera.GetUpVector() << '\n';
	os << "RIGHT::DIRECTION::VECTOR::" << camera.GetRightVector() << '\n';
	return os;
}

