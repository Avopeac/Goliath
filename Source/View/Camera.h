#pragma once
#include <GLFW\glfw3.h>
#include <GLM\glm.hpp>
#include <GLM\gtc\quaternion.hpp>
#include <GLM\gtx\quaternion.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#include "..\Drawable\Plane.h"
#include "..\Input\InputEnabled.h"
class Camera : public InputEnabled {
public:
	Camera(const glm::vec3 &eye, const glm::vec3 &forward, const glm::vec3 &world_up, double vertical_fov, double aspect_ratio, double near, double far);

	void update(double delta_time);

	const glm::mat4 &get_perspective() const { return _perspective; }
	const glm::mat4 &get_view() const { return _view; }

	double get_vertical_fov() const { return _vertical_fov; }
	double get_horizontal_fov() const { return _horizontal_fov; }
	double get_aspect_ratio() const { return _aspect_ratio; }

	double get_near() const { return _near; }
	double get_far() const { return _far; }

	bool intersects_point(const glm::vec3 &point);
	bool intersects_sphere(const glm::vec3 &center, double radius);
	bool intersects_box(const glm::vec3 &center, const glm::vec3 &extents);

private:
	double _vertical_fov, _horizontal_fov, _aspect_ratio;
	double _near, _far;
	double _accumulated_roll, _time;

	glm::mat4 _view, _perspective;
	glm::quat _rotation_quat, _next_rotation_quat;
	glm::quat _yaw, _pitch, _roll;
	glm::mat4 _translation, _rotation_mat;

	Plane _frustum[6];
	glm::vec3 _points[8];

	virtual void handle_mouse_movement(double x, double y, double delta_x, double delta_y, double acc_x, double acc_y, double delta_time) override;
	virtual void handle_multiple_keystrokes(GLFWwindow *window, double delta_time) override;
};