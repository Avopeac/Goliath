#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/quaternion.hpp>
#include <GLM/gtx/quaternion.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include "Drawable/Plane.h"
#include "Model/Vertex.h"
#include "Input/InputEnabled.h"

class Camera : public InputEnabled {
public:
	Camera(const glm::dvec3 &eye, const glm::dvec3 &center, const glm::dvec3 &world_up, double vertical_fov, double aspect_ratio, double near, double far);

	//Get double precision view data
	const glm::dmat4 &get_dview_proj() const { return _view_proj; }
	const glm::dmat4 &get_dprojection() const { return _projection; }
	const glm::dmat4 &get_dview() const { return _view; }
	const glm::dvec3 &get_deye() const { return _eye; }

	//Get float precision view data
	const glm::mat4 get_fview_proj() const { return glm::mat4(_view_proj); }
	const glm::mat4 get_fprojection() const { return glm::mat4(_projection); }
	const glm::mat4 get_fview() const { return glm::mat4(_view); }
	const glm::vec3 get_feye() const { return glm::vec3(_eye); }

	void update(double delta_time);

	//Get camera parameters
	double get_vertical_fov() const { return _vertical_fov; }
	double get_horizontal_fov() const { return _horizontal_fov; }
	double get_aspect_ratio() const { return _aspect_ratio; }
	double get_near() const { return _near; }
	double get_far() const { return _far; }

	//Intersection methods
	bool intersects_point(const glm::dvec3 &point) const;
	bool intersects_sphere(const glm::dvec3 &center, double radius) const;
	bool intersects_box(const glm::dvec3 &center, const glm::dvec3 &extents) const;

	//For debugging frustum
	//std::vector<Vertex> vertices;
	//std::vector<unsigned int> indices;

	void set_camera_speed(double speed) { _movement_speed = speed; }

private:
	double _time;

	//Camera parameters
	double _vertical_fov;
	double _horizontal_fov;
	double _aspect_ratio;
	double _near;
	double _far;

	//Orientation
	double _yaw;
	double _pitch;
	double _roll;
	double _movement_speed = 200.0;

	//View data
	glm::dmat4 _view_proj;
	glm::dmat4 _view;
	glm::dmat4 _projection;
	glm::dvec3 _eye;

	//Reference orienation
	glm::dvec3 _base_forward;
	glm::dvec3 _base_right;
	glm::dvec3 _base_up;
	glm::dquat _rotation_quat;

	//Next orientation
	glm::dvec3 _next_forward;
	glm::dvec3 _next_right;
	glm::dvec3 _next_up;
	glm::dquat _next_rotation_quat;

	//Frustum thingys
	void build_frustum();
	bool _update_frustum = true;
	glm::dvec3 _points[8];
	Plane _frustum[6];

	//Input "callbacks"
	virtual void handle_mouse_movement(double x, double y, double delta_x, double delta_y, double delta_time, bool captured) override;
	virtual void handle_multiple_keystrokes(GLFWwindow *window, double delta_time) override;
	virtual void handle_key_inputs(int key, int scan_code, int action, int mods, double delta_time) override;
};