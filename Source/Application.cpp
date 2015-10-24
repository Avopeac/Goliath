#pragma once
#include <iostream>
#include "View\Renderer.h"
#include "View\Camera.h"
#include "Application.h"
#include "Thread\MessageSystem.h"
#include "Input\Input.h"
//TO BE REMOVED
#include "Model\Shader.h"
#include "Drawable\Sphere.h"

Application::Application(unsigned int width, unsigned int height, const std::string &title) : _width(width), _height(height), _title(title) {}

int Application::initialize() {
	//Accumulate initialization status
	int status = 0;
	status += initialize_glfw(3, 3);
	status += initialize_glew(true);
	if (status < 0) {
		glfwTerminate();
	}

	//White clear
	_clear_color = glm::vec4(1.0, 1.0, 1.0, 1.0);

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
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_SAMPLES, 4);

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

	Shader shader("Shaders/phong.vert", "Shaders/phong.frag");
	std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(glm::vec3(0,0,-2), 0.11);
	sphere->set_shader(shader);
	sphere->generate_mesh(25, 25);

	Camera camera(glm::vec3(0, 0, -2), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), 45.0, (double)_width/_height, 0.1, 1000.0);
	Input input(_window_ptr);
	input.add_input_enabled_object(&camera);

	//Set viewport settings
	glViewport(0, 0, _width, _height);
	glClearColor(_clear_color.r, _clear_color.g, _clear_color.b, _clear_color.a);
	
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
		Renderer::instance().add_drawable(sphere);
		Renderer::instance().render(camera, _delta_time);

		glfwSwapInterval(1);
		glfwSwapBuffers(_window_ptr);
		glfwPollEvents();
	}
	//Stop the threads
	MessageSystem::instance().clean_up();
	//Shut down glfw
	glfwTerminate();
}