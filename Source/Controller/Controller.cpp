#include "..\Application.h"
#include "Controller.h"
#include <iostream>

Controller Controller::instance;

void Controller::Initialize(GLFWwindow *window) {
	glfwSetCursorPosCallback(window, &Controller::CursorCallback);
	glfwSetScrollCallback(window, &Controller::ScrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	double centerX = floor(Application::GetWindowWidth() * 0.5);
	double centerY = floor(Application::GetWindowHeight() * 0.5);
	lastMousePosition.x = (float)centerX;
	lastMousePosition.y = (float)centerY;

	this->window = window;
}

unsigned short Controller::GetCount() {
	return keyBindings.size();
}

KeyAction::ActionType Controller::PeekAction() {
	if (!actionQueue.empty()) { return actionQueue.front(); }

	return KeyAction::ActionType::NO_ACTION;
}

void Controller::ConsumeAction() {
	if (!actionQueue.empty()) {	actionQueue.pop(); }
}

void Controller::AddKeyBind(KeyAction action) {
	keyBindings.insert({ action.GetKey(), action.GetActionType() });
}

KeyAction::ActionType Controller::FetchAction(KeyAction::Key key) {
	auto it = keyBindings.find(key);
	if (it != keyBindings.end()) { return (*it).second; }

	return KeyAction::ActionType::NO_ACTION;
}

unsigned short Controller::UpdateContinuousInput() {
	unsigned short count = 0;
	for (auto it = keyBindings.cbegin(); it != keyBindings.end(); ++it) {
		int key = (*it).first.code;
		if (glfwGetKey(window, (*it).first.code) == GLFW_PRESS) {
			this->actionQueue.push((*it).second);
			count++;
		}
	}

	double x, y;
	glfwGetCursorPos(window, &x, &y);
	if (lastMousePosition.x == floor(x) &&
		lastMousePosition.y == floor(y)) {
		mouseDelta.x = 0;
		mouseDelta.y = 0;
	}

	return count;
}

void Controller::ScrollCallbackImpl(GLFWwindow *window, double x, double y) {
	mouseScroll.x = (float)floor(x);
	mouseScroll.y = (float)floor(y);
}

void Controller::CursorCallbackImpl(GLFWwindow *window, double x, double y) {

	float xPos = (float)floor(x);
	float yPos = (float)floor(y);

	if (firstMouseInput) {
		lastMousePosition.x = xPos;
		lastMousePosition.y = yPos;
		firstMouseInput = false;
	}

	mouseDelta.x = xPos - lastMousePosition.x;
	mouseDelta.y = lastMousePosition.y - yPos;
	lastMousePosition.x = xPos;
	lastMousePosition.y = yPos;
}

//Callback wrappers
void Controller::ScrollCallback(GLFWwindow *window, double x, double y) { GetInstance().ScrollCallbackImpl(window, x, y); }
void Controller::CursorCallback(GLFWwindow *window, double x, double y) { GetInstance().CursorCallbackImpl(window, x, y); }