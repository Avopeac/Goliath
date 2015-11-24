#pragma once
#include <vector>
#include <GLFW\glfw3.h>
#include <GLM\glm.hpp>
#include <GLM\gtc\quaternion.hpp>
#include <GLM\gtx\quaternion.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#include "..\Drawable\Plane.h"
#include "..\Model\Vertex.h"
#include "..\Input\InputEnabled.h"

class Camera : public InputEnabled {
public:
	Camera(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &world_up, double vertical_fov, double aspect_ratio, double near, double far);
	void update(double delta_time);
	const glm::mat4 &get_perspective() const { return _perspective; }
	const glm::mat4 &get_view() const { return _view; }
	double get_vertical_fov() const { return _vertical_fov; }
	double get_horizontal_fov() const { return _horizontal_fov; }
	double get_aspect_ratio() const { return _aspect_ratio; }
	double get_near() const { return _near; }
	double get_far() const { return _far; }
	const glm::vec3 &get_eye() const { return _eye; }
	bool intersects_point(const glm::vec3 &point) const;
	bool intersects_sphere(const glm::vec3 &center, double radius) const;
	bool intersects_box(const glm::vec3 &center, const glm::vec3 &extents) const;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	void set_camera_speed(double speed) { _movement_speed = speed; }

private:
	double _vertical_fov, _horizontal_fov, _aspect_ratio;
	double _near, _far;
	double _time;
	double _yaw, _pitch, _roll;
	double _movement_speed = 25.0;
	glm::mat4 _view, _perspective;
	glm::quat _rotation_quat, _next_rotation_quat;
	glm::vec3 _eye, _base_forward, _base_right, _base_up;
	glm::vec3 _next_forward, _next_right, _next_up;

	void build_frustum();
	bool _update_frustum = true;
	Plane _frustum[6];
	glm::vec3 _points[8];

	virtual void handle_mouse_movement(double x, double y, double delta_x, double delta_y, double delta_time, bool captured) override;
	virtual void handle_multiple_keystrokes(GLFWwindow *window, double delta_time) override;
	virtual void handle_key_inputs(int key, int scan_code, int action, int mods, double delta_time) override;
};