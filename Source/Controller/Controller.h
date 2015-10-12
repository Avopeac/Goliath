#pragma once
#include <map>
#include <queue>
#include <GLFW\glfw3.h>
#include <GLM\glm.hpp>
#include "..\Controller\KeyAction.h"
/// A controller handles keystrokes and mouse movement. It is a singleton pattern. 
/// The controller makes use of the KeyAction class to map keystrokes to actions.
/// The actions are put upon the stack and then the actions are processed each frame.
class Controller {
public:
	//Singleton
	static Controller &instance() { 
		static Controller _instance;
		return _instance; 
	}
	//Set callback functions for input 
	void initialize(GLFWwindow *window);
	//Add a key action
	void add_key_bind(KeyAction action);
	//What is the next action?
	KeyAction::ActionType peek_action();
	//Call when the action has been used
	void consume_action();
	unsigned short update_continuous_input();
	unsigned short get_count();
	//callback functions
	static void cursor_callback(GLFWwindow *window, double x, double y);
	static void scroll_callback(GLFWwindow *window, double x, double y);
	//callback function implementation
	void cursor_callback_impl(GLFWwindow *window, double x, double y);
	void scroll_callback_impl(GLFWwindow *window, double x, double y);
	//Get mouse scroll offset and resets it
	const glm::vec2 &get_mouse_scroll() const { return _mouse_scroll; }
	const glm::vec2 &get_mouse_position() const { return _current_mouse_position; }
	const glm::vec2 &get_last_mouse_position() const { return _last_mouse_position; }
	const glm::vec2 &get_mouse_delta() const { return _mouse_delta; }
	//If the key is in the map, return the corresponding action type
	KeyAction::ActionType fetch_action(KeyAction::Key key);

private:
	Controller() = default;
	Controller(const Controller &) = delete;
	Controller &operator=(const Controller &) = default;
	std::map<KeyAction::Key, KeyAction::ActionType> _key_bindings;
	std::queue<KeyAction::ActionType> _action_queue;
	glm::vec2 _last_mouse_position;
	glm::vec2 _current_mouse_position;
	glm::vec2 _mouse_delta;
	glm::vec2 _mouse_scroll;
	GLFWwindow *_window_ptr;
	bool _first_mouse_input = true;
};