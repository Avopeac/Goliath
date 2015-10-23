#pragma once
#include <GLFW\glfw3.h>
#include <vector>
#include "..\Input\InputEnabled.h"
class Input {
public:
	Input(GLFWwindow *window);
	void add_input_enabled_object(InputEnabled *object);
	void update(double delta_time);

private:
	static int _key, _scan_code, _key_action, _key_mods;
	static double _x_pos, _y_pos, _delta_x, _delta_y, _acc_x, _acc_y;
	static int _entered;
	static int _button, _mouse_action, _mouse_mods;
	static double _offset_x, _offset_y;
	std::vector<InputEnabled*> _objects;
	GLFWwindow *_window_ptr;
	friend void key_callback(GLFWwindow *window, int key, int scan_code, int action, int mods);
	friend void cursor_position_callback(GLFWwindow *window, double x_pos, double y_pos);
	friend void cursor_enter_callback(GLFWwindow *window, int entered);
	friend void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
	friend void scroll_callback(GLFWwindow *window, double offset_x, double offset_y);

};
