#include "..\Application.h"
#include "Controller.h"
#include <iostream>

void Controller::initialize(GLFWwindow *window) {
	glfwSetCursorPosCallback(window, &Controller::cursor_callback);
	glfwSetScrollCallback(window, &Controller::scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	double center_x = floor(Application::get_window_width() * 0.5);
	double center_y = floor(Application::get_window_height() * 0.5);
	_last_mouse_position.x = (float)center_x;
	_last_mouse_position.y = (float)center_y;
	_window_ptr = window;
}

unsigned short Controller::get_count() {
	return _key_bindings.size();
}

KeyAction::ActionType Controller::peek_action() {
	if (!_action_queue.empty()) { return _action_queue.front(); }
	return KeyAction::ActionType::NO_ACTION;
}

void Controller::consume_action() {
	if (!_action_queue.empty()) { _action_queue.pop(); }
}

void Controller::add_key_bind(KeyAction action) {
	_key_bindings.insert({ action.get_key(), action.get_action_type() });
}

KeyAction::ActionType Controller::fetch_action(KeyAction::Key key) {
	auto it = _key_bindings.find(key);
	if (it != _key_bindings.end()) { return (*it).second; }
	return KeyAction::ActionType::NO_ACTION;
}

unsigned short Controller::update_continuous_input() {
	unsigned short count = 0;
	for (auto it = _key_bindings.cbegin(); it != _key_bindings.end(); ++it) {
		int key = (*it).first._code;
		if (glfwGetKey(_window_ptr, (*it).first._code) == GLFW_PRESS) {
			_action_queue.push((*it).second);
			count++;
		}
	}
	double x, y;
	glfwGetCursorPos(_window_ptr, &x, &y);
	if (_last_mouse_position.x == floor(x) &&
		_last_mouse_position.y == floor(y)) {
		_mouse_delta.x = 0;
		_mouse_delta.y = 0;
	}
	return count;
}

void Controller::scroll_callback_impl(GLFWwindow *window, double x, double y) {
	_mouse_scroll.x = (float)floor(x);
	_mouse_scroll.y = (float)floor(y);
}

void Controller::cursor_callback_impl(GLFWwindow *window, double x, double y) {
	float x_pos = (float)floor(x);
	float y_pos = (float)floor(y);
	if (_first_mouse_input) {
		_last_mouse_position.x = x_pos;
		_last_mouse_position.y = y_pos;
		_first_mouse_input = false;
	}
	_mouse_delta.x = x_pos - _last_mouse_position.x;
	_mouse_delta.y = _last_mouse_position.y - y_pos;
	_last_mouse_position.x = x_pos;
	_last_mouse_position.y = y_pos;
}

//Callback wrappers
void Controller::scroll_callback(GLFWwindow *window, double x, double y) { instance().scroll_callback_impl(window, x, y); }
void Controller::cursor_callback(GLFWwindow *window, double x, double y) { instance().cursor_callback_impl(window, x, y); }