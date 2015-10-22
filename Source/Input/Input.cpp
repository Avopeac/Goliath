#include "Input.h"

Input::Input(GLFWwindow *window) {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorEnterCallback(window, cursor_enter_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

void Input::add_input_enabled_object(InputEnabled *object) {
	_objects.push_back(object);
}

void Input::update(double delta_time) {
	for (auto it = _objects.begin(); it != _objects.end(); ++it) {
		InputEnabled *ptr = *it;
		ptr->handle_key_inputs(_key, _scan_code, _key_action, _key_mods, delta_time);
		ptr->handle_mouse_entered(_entered, delta_time);
		ptr->handle_mouse_movement(_x_pos, _y_pos, _delta_x, _delta_y, _acc_x, _acc_y, delta_time);
		ptr->handle_mouse_press(_button, _mouse_action, _mouse_mods, delta_time);
		ptr->handle_scroll_actions(_offset_x, _offset_y, delta_time);

		if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
			int axes_count, state_count;
			const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
			const unsigned char *state = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &state_count);
			const char *name = glfwGetJoystickName(GLFW_JOYSTICK_1);
			ptr->handle_joystick(axes, axes_count, state, state_count, name, delta_time);
		}
	}
}

void key_callback(GLFWwindow * window, int key, int scan_code, int action, int mods) {
	Input::_key = key;
	Input::_scan_code = scan_code;
	Input::_key_action = action;
	Input::_key_mods = mods;
}

void cursor_position_callback(GLFWwindow * window, double x_pos, double y_pos) {
	Input::_delta_x = Input::_x_pos - x_pos;
	Input::_delta_y = Input::_y_pos - y_pos;
	Input::_acc_x += Input::_delta_x;
	Input::_acc_y += Input::_delta_y;
	Input::_x_pos = x_pos;
	Input::_y_pos = y_pos;
}

void cursor_enter_callback(GLFWwindow * window, int entered) {
	Input::_entered = entered;
}

void mouse_button_callback(GLFWwindow * window, int button, int action, int mods) {
	Input::_button = button;
	Input::_mouse_action = action;
	Input::_mouse_mods = mods;
}

void scroll_callback(GLFWwindow * window, double offset_x, double offset_y) {
	Input::_offset_x = offset_x;
	Input::_offset_y = offset_y;
}

//Mouse 
int Input::_mouse_action, Input::_button, Input::_entered, Input::_mouse_mods;
double Input::_x_pos, Input::_y_pos, Input::_acc_x, Input::_acc_y, Input::_delta_x, Input::_delta_y;
//Keyboard
int Input::_key_action, Input::_key, Input::_scan_code, Input::_key_mods;
//Scroll
double Input::_offset_x, Input::_offset_y;
