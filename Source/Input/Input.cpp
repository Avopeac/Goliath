#include "Input.h"
#include <iostream>
#include <GLM\glm.hpp>
#include "AntTweakBar/AntTweakBar.h"

Input::Input(GLFWwindow *window, std::string tweak_bar_name) {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorEnterCallback(window, cursor_enter_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	_window_ptr = window;
	//Create AntTweakBar
	_tw_bar = TwNewBar(tweak_bar_name.c_str());
}

void Input::add_input_enabled_object(InputEnabled *object) {
	_objects.push_back(object);
}

void Input::update(double delta_time) {
	for (auto it = _objects.begin(); it != _objects.end(); ++it) {
		InputEnabled *ptr = *it;
		ptr->handle_key_inputs(_key, _scan_code, _key_action, _key_mods, delta_time);
		ptr->handle_mouse_entered(_entered, delta_time, _mouse_captured);
		ptr->handle_mouse_movement(_x_pos, _y_pos, _delta_x, _delta_y, delta_time, _mouse_captured);
		ptr->handle_mouse_press(_button, _mouse_action, _mouse_mods, delta_time, _mouse_captured);
		ptr->handle_scroll_actions(_offset_x, _offset_y, delta_time);

		if (_window_ptr != nullptr) {
			ptr->handle_multiple_keystrokes(_window_ptr, delta_time);
		}

		if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
			int axes_count, state_count;
			const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
			const unsigned char *state = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &state_count);
			const char *name = glfwGetJoystickName(GLFW_JOYSTICK_1);
			ptr->handle_joystick(axes, axes_count, state, state_count, name, delta_time);
		}
	}

	//Capture cursor, for GUI purposes
	_mouse_captured = !glfwGetKey(_window_ptr, GLFW_KEY_LEFT_CONTROL);
	if (!_mouse_captured && (glfwGetInputMode(_window_ptr, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)) {
		glfwSetInputMode(_window_ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else if (_mouse_captured && (glfwGetInputMode(_window_ptr, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)) {
		glfwSetInputMode(_window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	Input::_delta_x = 0;
	Input::_delta_y = 0;
}

void key_callback(GLFWwindow * window, int key, int scan_code, int action, int mods) {
	Input::_key = key;
	Input::_scan_code = scan_code;
	Input::_key_action = action;
	Input::_key_mods = mods;
	Input::_tw_key_callback_handled = TwEventKeyGLFW(key, action);
}

void cursor_position_callback(GLFWwindow * window, double x_pos, double y_pos) {
	Input::_delta_x = Input::_x_pos - x_pos;
	Input::_delta_y = Input::_y_pos - y_pos;
	Input::_x_pos = x_pos;
	Input::_y_pos = y_pos;
	Input::_tw_cursor_position_callback_handled = TwEventMousePosGLFW((int)x_pos, (int)y_pos);
}

void cursor_enter_callback(GLFWwindow * window, int entered) {
	Input::_entered = entered;
}

void mouse_button_callback(GLFWwindow * window, int button, int action, int mods) {
	Input::_button = button;
	Input::_mouse_action = action;
	Input::_mouse_mods = mods;
	Input::_tw_mouse_button_callback_handled = TwEventMouseButtonGLFW(button, action);
}

void scroll_callback(GLFWwindow * window, double offset_x, double offset_y) {
	Input::_offset_x = offset_x;
	Input::_offset_y = offset_y;
	Input::_tw_scroll_callback_handled = TwEventMouseWheelGLFW((int)offset_y);
}

//Mouse 
int Input::_mouse_action, Input::_button, Input::_entered, Input::_mouse_mods;
bool Input::_mouse_captured = true;
double Input::_x_pos, Input::_y_pos, Input::_delta_x, Input::_delta_y;
//Keyboard
int Input::_key_action, Input::_key, Input::_scan_code, Input::_key_mods;
//Scroll
double Input::_offset_x, Input::_offset_y;
//AntTweakBar
TwBar* Input::_tw_bar;
int Input::_tw_cursor_position_callback_handled, Input::_tw_key_callback_handled, Input::_tw_mouse_button_callback_handled, Input::_tw_scroll_callback_handled;