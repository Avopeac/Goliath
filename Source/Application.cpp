#pragma once
#include <iostream>
#include <iterator>
#include "Application.h"
#include "Controller\Controller.h"
#include "Visibility\Camera.h"
#include "QuadTree\Tile.h"
#include "QuadTree\QuadTree.h"
#include "Model\Shader.h"
#include "Math\MathHelp.h"
#include "MultiThreading\MessageSystem.h"

unsigned int Application::_window_width = WINDOW_WIDTH;
unsigned int Application::_window_height = WINDOW_HEIGHT;
std::string Application::_window_title = WINDOW_NAME;
double Application::_delta_time = 0.0;

Application::Application(unsigned int width, unsigned int height, std::string title) {
	_window_width = width;
	_window_height = height;
	_window_title = title;
}

int Application::initialize() {
	//Accumulate initialization status
	int status = 0;
	status += initialize_glfw(3, 3);
	status += initialize_glew(true);
	if (status < 0) {
		glfwTerminate();
	}
	//Set up first person controller
	Controller &controller = Controller::instance();
	controller.initialize(_window_ptr);
	//Add camera controllers
	controller.add_key_bind(KeyAction(KeyAction::Key(GLFW_KEY_W), KeyAction::ActionType::MOVE_FORWARD));
	controller.add_key_bind(KeyAction(KeyAction::Key(GLFW_KEY_S), KeyAction::ActionType::MOVE_BACK));
	controller.add_key_bind(KeyAction(KeyAction::Key(GLFW_KEY_A), KeyAction::ActionType::MOVE_LEFT));
	controller.add_key_bind(KeyAction(KeyAction::Key(GLFW_KEY_D), KeyAction::ActionType::MOVE_RIGHT));
	controller.add_key_bind(KeyAction(KeyAction::Key(GLFW_KEY_Q), KeyAction::ActionType::MOVE_UP));
	controller.add_key_bind(KeyAction(KeyAction::Key(GLFW_KEY_E), KeyAction::ActionType::MOVE_DOWN));
	controller.add_key_bind(KeyAction(KeyAction::Key(GLFW_KEY_KP_1), KeyAction::ActionType::INCREASE_SPEED));
	controller.add_key_bind(KeyAction(KeyAction::Key(GLFW_KEY_KP_3), KeyAction::ActionType::DECREASE_SPEED));
	//Add light controllers
	controller.add_key_bind(KeyAction(KeyAction::Key(GLFW_KEY_KP_8), KeyAction::ActionType::LIGHT_FORWARD));
	controller.add_key_bind(KeyAction(KeyAction::Key(GLFW_KEY_KP_2), KeyAction::ActionType::LIGHT_BACK));
	controller.add_key_bind(KeyAction(KeyAction::Key(GLFW_KEY_KP_4), KeyAction::ActionType::LIGHT_LEFT));
	controller.add_key_bind(KeyAction(KeyAction::Key(GLFW_KEY_KP_6), KeyAction::ActionType::LIGHT_RIGHT));
	controller.add_key_bind(KeyAction(KeyAction::Key(GLFW_KEY_KP_7), KeyAction::ActionType::LIGHT_UP));
	controller.add_key_bind(KeyAction(KeyAction::Key(GLFW_KEY_KP_9), KeyAction::ActionType::LIGHT_DOWN));
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
	_window_ptr = glfwCreateWindow(_window_width, _window_height, _window_title.c_str(), nullptr, nullptr);
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
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.52f, 0.8f, 0.95f, 1.0f);
	//Enable depth testing and depth clamping for non-linear z-buffer
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	//Enable back-face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//TODO: Remove all the testing stuff!
	double radius = 6378137.0 * 0.5;
	Camera camera(0, radius * 5.0, 0);
	camera.set_perspective(45.0, (double)Application::_window_width / Application::_window_height, 0.01, 27000000.0);
	QuadTree tree(AABB<double>(glm::highp_vec3(0,0,0), radius));

	Shader shader("Shaders\\terrain.vert", "Shaders\\terrain.frag");

	glm::vec3 light_source(0, 1, 0);
	
	double timer = 0.0;
	bool is_changed = false;
	double old_time = 0.0;

	Controller &controller = Controller::instance();

	while (!glfwWindowShouldClose(_window_ptr)) {
		//Poll events first
		glfwPollEvents();
		//Set delta time
		double current_time = glfwGetTime();
		_delta_time = current_time - old_time;
		old_time = current_time;
		//Set frames per second
		int frames = (int)(1.0 / _delta_time);
		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//TODO: Remove all the testing stuff!
		camera.update((float)_delta_time);
		glm::mat4 model, projection, view;
		projection = camera.get_projection_matrix();
		view = camera.get_view_matrix();
		controller.update_continuous_input();
		bool changed = false;
		for (int i = 0; i < controller.get_count(); ++i) {
			KeyAction::ActionType t = controller.peek_action();
			if (t == KeyAction::ActionType::LIGHT_BACK) { light_source.z -= _delta_time; changed = true; }
			if (t == KeyAction::ActionType::LIGHT_FORWARD) { light_source.z += _delta_time; changed = true; }
			if (t == KeyAction::ActionType::LIGHT_LEFT) { light_source.x -= _delta_time; changed = true; }
			if (t == KeyAction::ActionType::LIGHT_RIGHT) { light_source.x += _delta_time; changed = true; }
			if (t == KeyAction::ActionType::LIGHT_UP) { light_source.y += _delta_time; changed = true; }
			if (t == KeyAction::ActionType::LIGHT_DOWN) { light_source.y -= _delta_time; changed = true; }
			if (changed) {
				controller.consume_action();
			}
		}

		glm::mat4 relative_to_eye(model * view);
		relative_to_eye[3][0] = 0;
		relative_to_eye[3][1] = 0;
		relative_to_eye[3][2] = 0;

		float xp, xl, yp, yl, zp, zl;
		MathHelp::double_to_two_floats(camera.get_eye_vector().x, xp, xl);
		MathHelp::double_to_two_floats(camera.get_eye_vector().y, yp, yl);
		MathHelp::double_to_two_floats(camera.get_eye_vector().z, zp, zl);

		shader.use();
		glUniform3fv(glGetUniformLocation(shader._program, "light"), 1, value_ptr(light_source));
		glUniform1i(glGetUniformLocation(shader._program, "logarithmicDepth"), true);
		glUniform1f(glGetUniformLocation(shader._program, "logarithmicConstant"), 0.1);
		glUniform1f(glGetUniformLocation(shader._program, "nearPlaneDistance"), 0.01);
		glUniformMatrix4fv(glGetUniformLocation(shader._program, "relativeToEye"), 1, GL_FALSE, value_ptr(relative_to_eye));
		glUniform3fv(glGetUniformLocation(shader._program, "eyeVecHigh"), 1, value_ptr(glm::vec3(xp, yp, zp)));
		glUniform3fv(glGetUniformLocation(shader._program, "eyeVecLow"), 1, value_ptr(glm::vec3(xl, yl, zl)));
		glUniformMatrix4fv(glGetUniformLocation(shader._program, "proj"), 1, GL_FALSE, value_ptr(projection));
		tree.draw(shader, camera);
	
		//Double buffering
		glfwSwapBuffers(_window_ptr);
	}

	//Clean up the job scheduler threads
	MessageSystem::instance().clean_up();
	//Shut down glfw
	glfwTerminate();
}