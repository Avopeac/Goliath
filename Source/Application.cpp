#pragma once
#include <iostream>
#include "Renderer.h"
#include "Application.h"
#include "MessageSystem.h"
Application::Application(unsigned int width, unsigned int height, const std::string &title) : _width(width), _height(height), _title(title) {}

int Application::initialize() {
	//Accumulate initialization status
	int status = 0;
	status += initialize_glfw(3, 3);
	status += initialize_glew(true);
	if (status < 0) {
		glfwTerminate();
	}
	//Initialize the message passing system
	MessageSystem::instance();
	return status;
}

int Application::initialize_glfw(int major_version, int minor_version) {
	//Initialize glfw and set OpenGL version
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//Create a OpenGL context
	_window_ptr = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
	if (_window_ptr == nullptr) {
		std::cout << "Application failed to create a OpenGL context with GLFW. " << std::endl;
		return -1;
	}
	//Set context and vsync
	glfwMakeContextCurrent(_window_ptr);
	glfwSwapInterval(1);
	std::cout << "Application successfully initialized GLFW and set up OpenGL context. " << std::endl;
	return 1;
}

int Application::initialize_glew(bool experimental) {
	//Initialize glew, use experimental (ARB etc.)
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Application failed to initialize GLEW. " << std::endl;
		return -1;
	}
	std::cout << "Application successfully initialized GLEW. " << std::endl;
 	return 1;
}

void Application::run() {
	//Set viewport settings
	glViewport(0, 0, _width, _height);
	glClearColor(_clear_color.r, _clear_color.g, _clear_color.b, _clear_color.a);
	//Loop
	double old_time = 0.0;
	while (!glfwWindowShouldClose(_window_ptr)) {
		//Poll events
		glfwPollEvents();
		//Timings
		_elapsed_time = glfwGetTime();
		_delta_time = _elapsed_time - old_time;
		old_time = _elapsed_time;
		_frames_per_second = 1.0 / _delta_time;
		//Start rendering
		Renderer::instance().render(_delta_time);
		//Swap backbuffer
		glfwSwapBuffers(_window_ptr);
	}
	//Stop the threads
	MessageSystem::instance().clean_up();
	//Shut down glfw
	glfwTerminate();
}