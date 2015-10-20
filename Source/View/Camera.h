#pragma once
#include <iostream>
#include <GLM\glm.hpp>
#include <GLM\mat4x4.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtx\transform.hpp>
#include "..\Application.h"
#include "..\Controller\Controller.h"
#include "Frustum.h"
//Maximum amount of vertical viewing angle
#ifndef MAX_Y_ROTATION_DEGREES
#define MAX_Y_ROTATION_DEGREES 89.999f
#endif
//Minimum amount of vertical viewing angle
#ifndef MIN_Y_ROTATION_DEGREES
#define MIN_Y_ROTATION_DEGREES -89.999f
#endif
///A camera is what views the scene. 
///We don't actually move the camera in the world. We move to world in front of the camera. 
///This is a very helpful thing to keep in mind when doing transformations.
///A better version of this would be to use quaternions to avoid locking.
class Camera {
public:
	Camera(double x = 0.0, double y = 0.0, double z = 0.0);
	Camera(glm::dvec3 position);
	~Camera();
	//Set camera orientation
	void set_yaw(double degrees);
	void set_pitch(double degrees);
	void set_roll(double degrees);
	void set_position(glm::dvec3 position);
	void set_position(double x, double y, double z);
	//Set camera parameters
	void set_orthographic(double left, double right, double bottom, double top);
	void set_perspective(double fov_y, double aspect, double z_near, double z_far);
	void set_perspective(double fov_y, double width, double height, double z_near, double z_far);
	//Runs every frame
	void update(float delta_time);
	//Getters for camera parameters
	const double get_fov_y() const { return _fov_y; }
	const double get_fov_x() const { return _fov_x; }
	const double get_aspect() const { return _aspect; }
	const double get_near() const { return _near; }
	const double get_far() const { return _far; }
	//Getters for camera orientation
	const glm::dvec3 &get_eye_vector() const { return _eye; }
	const glm::dvec3 &get_look_vector() const { return _look; }
	const glm::dvec3 &get_up_vector() const { return _up; }
	const glm::dvec3 &get_right_vector() const { return _right; }
	const double get_yaw() const { return _yaw; }
	const double get_pitch() const { return _pitch; }
	const double get_roll() const { return _roll; }
	//Getters for transforms
	const glm::dmat4 &get_projection_matrix() const { return _projection; }
	const glm::dmat4 &get_view_matrix() const { return _view; }
	//Getter for frustum
	Frustum &get_frustum() { return _camera_frustum; }

protected:
	//Frustum for checking camera view collisions
	Frustum _camera_frustum;
	//Camera orientation matrices
	glm::dmat4 _rotation;
	glm::dmat4 _projection;
	glm::dmat4 _view;
	//Camera parameters
	double _fov_y = .0;
	double _fov_x = .0;
	double _aspect = .0;
	double _near = .0;
	double _far = .0;
	//Camera orientation vectors
	glm::dvec3 _eye = glm::dvec3(0, 0, 0);
	glm::dvec3 _look = glm::dvec3(0, 0, -1);
	glm::dvec3 _right = glm::dvec3(1, 0, 0);
	glm::dvec3 _up = glm::dvec3(0, 1, 0);
	//Camera orientation values
	double _yaw = .0;
	double _pitch = .0;
	double _roll = .0;
	//Camera translation values
	double _translation_speed = 999999.0;
	double _direction_speed = 0.5;
};

//Out stream for vector type
inline std::ostream& operator<<(std::ostream &os, glm::vec3 vector) {
	os << "[" << vector.x << ", " << vector.y << ", " << vector.z << "]";
	return os;
}

//Formatting for out stream
inline std::ostream& operator<<(std::ostream &os, Camera &camera) {
	os << "Field of view: " << camera.get_fov_y() << '\n';
	os << "Aspect ratio: " << camera.get_aspect() << '\n';
	os << "Near plane distance: " << camera.get_near() << '\n';
	os << "Far plane distance: " << camera.get_far() << '\n';
	os << "Pitch angle: " << camera.get_pitch() << '\n';
	os << "Yaw angle: " << camera.get_yaw() << '\n';
	os << "Roll angle: " << camera.get_roll() << '\n';
	os << "Eye vector: " << -camera.get_eye_vector() << '\n';
	os << "Look vector: " << camera.get_look_vector() << '\n';
	os << "Up vector: " << camera.get_up_vector() << '\n';
	os << "Right vector: " << camera.get_right_vector() << '\n';
	return os;
}

