#pragma once
#include "..\Controller\KeyAction.h"
#include <GLFW\glfw3.h>
#include <GLM\glm.hpp>
#include <map>
#include <queue>

/// A controller handles keystrokes and mouse movement. It is a singleton pattern. 
/// The controller makes use of the KeyAction class to map keystrokes to actions.
/// The actions are put upon the stack and then the actions are processed each frame.
class Controller {

public:

	//Singleton
	static Controller& GetInstance() { return instance; }

	//Set callback functions for input 
	void Initialize(GLFWwindow *window);

	//Add a key action
	void AddKeyBind(KeyAction action);

	//What is the next action?
	KeyAction::ActionType PeekAction();

	//Call when the action has been used
	void ConsumeAction();
	unsigned short UpdateContinuousInput();
	unsigned short GetCount();

	//callback functions
	static void CursorCallback(GLFWwindow *window, double x, double y);
	static void ScrollCallback(GLFWwindow *window, double x, double y);

	//callback function implementation
	void CursorCallbackImpl(GLFWwindow *window, double x, double y);
	void ScrollCallbackImpl(GLFWwindow *window, double x, double y);

	//Get mouse scroll offset and resets it
	const glm::vec2 &GetMouseScroll() const { return mouseScroll; }
	const glm::vec2 &GetMousePosition() const { return currentMousePosition; }
	const glm::vec2 &GetLastMousePosition() const { return lastMousePosition; }
	const glm::vec2 &GetMouseDelta() const { return mouseDelta; }

	//If the key is in the map, return the corresponding action type
	KeyAction::ActionType FetchAction(KeyAction::Key key);

private:

	Controller() = default;
	Controller(const Controller &) = delete;
	Controller &operator=(const Controller &) = default;

	std::map<KeyAction::Key, KeyAction::ActionType> keyBindings;
	std::queue<KeyAction::ActionType> actionQueue;

	glm::vec2 lastMousePosition;
	glm::vec2 currentMousePosition;
	glm::vec2 mouseDelta;
	glm::vec2 mouseScroll;

	static Controller instance;
	GLFWwindow *window;
	bool firstMouseInput = true;
};