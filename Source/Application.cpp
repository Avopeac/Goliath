#pragma once
#include <iostream>
#include "Application.h"
#include "View\Renderer.h"
#include "View\Camera.h"
#include "View\ShaderStore.h"
#include "Terrain\Planet.h"
#include "Thread\MessageSystem.h"
#include "Input\Input.h"
#include "AntTweakBar\AntTweakBar.h"

unsigned int Application::width = WINDOW_WIDTH;
unsigned int Application::height = WINDOW_HEIGHT;

Application::Application(unsigned int width, unsigned int height, const std::string &title) : _title(title) {
	this->width = width;
	this->height = height;
}

int Application::initialize() {
	//Accumulate initialization status
	int status = 0;
	status += initialize_glfw(3, 3);
	status += initialize_glew(true);
	if (status < 0) {
		glfwTerminate();
	}
	//Initialize AntTweakBar
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(width, height);
	//Initialize the message passing system
	MessageSystem::instance();
	return status;
}

int Application::initialize_glfw(int major_version, int minor_version) {
	//Initialize glfw and set OpenGL version
	glfwInit();
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//Create a OpenGL context
	_window_ptr = glfwCreateWindow(width, height, _title.c_str(), nullptr, nullptr);
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
	//Load resources
	ShaderStore::instance().stock_inventory();
	//Create input manager
	Input input(_window_ptr, "TweakBar");
	//Create a renderer
	Renderer::instance().initialize();
	//Create planet
	std::shared_ptr<Planet> planet = std::make_shared<Planet>(4.0f);
	//Create camera
	Camera camera(glm::vec3(0, 0, 13), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 45.0, (double)width / height, 0.00000000001, 1000.0);
	//Add camera to as a input enabled object
	input.add_input_enabled_object(&camera);

	//Set viewport settings
	glViewport(0, 0, width, height);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	const double desired_time = 1.0 / 60.0;
	double old_time = 0.0;
	double accumulated_time = 0.0;
	while (!glfwWindowShouldClose(_window_ptr)) {
		//Timings
		_elapsed_time = glfwGetTime();
		_delta_time = _elapsed_time - old_time;
		old_time = _elapsed_time;
		_frames_per_second = 1.0 / _delta_time;
		accumulated_time += _delta_time;
		while (accumulated_time >= desired_time)
			accumulated_time -= desired_time;
		input.update(_delta_time);
		camera.update(_delta_time);
		//Add drawables and render
		Renderer::instance().add_drawable(planet);
		Renderer::instance().render(camera, _delta_time);
		glfwSwapInterval(1);
		glfwSwapBuffers(_window_ptr);
		glfwPollEvents();
	}
	//Stop the threads
	MessageSystem::instance().clean_up();
	//Clean up AntTweakBar	
	TwTerminate();
	//Shut down glfw 
	glfwTerminate();
}