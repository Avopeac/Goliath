#pragma once
#include <GLFW\glfw3.h>
#include <vector>
#include <AntTweakBar\AntTweakBar.h>
#include "InputEnabled.h"
class Input {
public:
	Input(GLFWwindow *window, std::string tweak_bar_name);
	void add_input_enabled_object(InputEnabled *object);
	void update(double delta_time);
	//AntTweakBar
	static TwBar *_tw_bar;
	static int _tw_key_callback_handled;
	static int _tw_cursor_position_callback_handled;
	static int _tw_mouse_button_callback_handled;
	static int _tw_scroll_callback_handled;

private:
	static int _key, _scan_code, _key_action, _key_mods;
	static double _x_pos, _y_pos, _delta_x, _delta_y;
	static int _entered;
	static int _button, _mouse_action, _mouse_mods;
	static double _offset_x, _offset_y;
	static bool _mouse_captured;
	std::vector<InputEnabled*> _objects;
	GLFWwindow *_window_ptr;
	friend void key_callback(GLFWwindow *window, int key, int scan_code, int action, int mods);
	friend void cursor_position_callback(GLFWwindow *window, double x_pos, double y_pos);
	friend void cursor_enter_callback(GLFWwindow *window, int entered);
	friend void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
	friend void scroll_callback(GLFWwindow *window, double offset_x, double offset_y);
};
