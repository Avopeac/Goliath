#pragma once
#include <GLFW\glfw3.h>
#include <GLM\glm.hpp>
#include "..\Drawable\Plane.h"
#include "..\Input\InputEnabled.h"
class Camera : public InputEnabled {
public:
	Camera(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up, double vertical_fov, double aspect_ratio, double near, double far);

	void update(double delta_time);

	const glm::mat4 &get_perspective() const { return _perspective; }
	const glm::mat4 &get_view() const { return _view; }

	double get_roll() const { return _roll; }
	double get_yaw() const { return _yaw; }
	double get_pitch() const { return _pitch; }

	double get_vertical_fov() const { return _vertical_fov; }
	double get_horizontal_fov() const { return _horizontal_fov; }
	double get_aspect_ratio() const { return _aspect_ratio; }

	double get_near() const { return _near; }
	double get_far() const { return _far; }

	bool intersects_point(const glm::vec3 &point);
	bool intersects_sphere(const glm::vec3 &center, double radius);
	bool intersects_box(const glm::vec3 &center, const glm::vec3 &extents);

protected:
	void set_roll(double amount);
	void set_pitch(double amount);
	void set_yaw(double amount);

private:
	double _vertical_fov, _horizontal_fov;
	double _aspect_ratio;
	double _near, _far;
	double _yaw, _pitch, _roll;
	glm::mat4 _perspective, _view;
	glm::vec3 _eye, _center, _up;

	Plane _frustum[6];
	glm::vec3 _points[8];

	virtual void handle_mouse_movement(double x, double y, double delta_x, double delta_y, double acc_x, double acc_y, double delta_time) override;
	virtual void handle_key_inputs(int key, int scan_code, int action, int mods, double delta_time) override;
};