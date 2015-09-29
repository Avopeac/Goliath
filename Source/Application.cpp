#pragma once
#include <iostream>
#include <iterator>

#include "Application.h"
#include "Controller\Controller.h"
#include "Visibility\Camera.h"
#include "QuadTree\Tile.h"
#include "Model\Shader.h"
#include "Math\MathHelp.h"


#define FAIL_GLFW_CREATE "FAILED_TO_CREATE_GLFW_WINDOW"
#define FAIL_GLEW_INIT "FAILED_TO_INITIALIZE_GLEW"

unsigned int Application::WINDOW_WIDTH = W_WIDTH;
unsigned int Application::WINDOW_HEIGHT = W_HEIGHT;
const char* Application::WINDOW_TITLE = W_NAME;
double Application::DELTA_TIME = 0.0;

Application::Application(unsigned int width, unsigned int height, const char *title) {
	WINDOW_WIDTH = width; WINDOW_HEIGHT = height; WINDOW_TITLE = title;
}

int Application::Initialize() {

	int status = 0;
	status += InitializeGLFW(3, 3);
	status += InitializeGLEW(true);
	if (status < 0) { glfwTerminate(); }

	//Set up first person controller
	Controller &controller = Controller::GetInstance();
	controller.Initialize(window);
	controller.AddKeyBind(KeyAction(KeyAction::Key(GLFW_KEY_W), KeyAction::ActionType::MOVE_FORWARD));
	controller.AddKeyBind(KeyAction(KeyAction::Key(GLFW_KEY_S), KeyAction::ActionType::MOVE_BACK));
	controller.AddKeyBind(KeyAction(KeyAction::Key(GLFW_KEY_A), KeyAction::ActionType::MOVE_LEFT));
	controller.AddKeyBind(KeyAction(KeyAction::Key(GLFW_KEY_D), KeyAction::ActionType::MOVE_RIGHT));
	controller.AddKeyBind(KeyAction(KeyAction::Key(GLFW_KEY_Q), KeyAction::ActionType::MOVE_UP));
	controller.AddKeyBind(KeyAction(KeyAction::Key(GLFW_KEY_E), KeyAction::ActionType::MOVE_DOWN));

	controller.AddKeyBind(KeyAction(KeyAction::Key(GLFW_KEY_KP_8), KeyAction::ActionType::LIGHT_FORWARD));
	controller.AddKeyBind(KeyAction(KeyAction::Key(GLFW_KEY_KP_2), KeyAction::ActionType::LIGHT_BACK));
	controller.AddKeyBind(KeyAction(KeyAction::Key(GLFW_KEY_KP_4), KeyAction::ActionType::LIGHT_LEFT));
	controller.AddKeyBind(KeyAction(KeyAction::Key(GLFW_KEY_KP_6), KeyAction::ActionType::LIGHT_RIGHT));
	controller.AddKeyBind(KeyAction(KeyAction::Key(GLFW_KEY_KP_7), KeyAction::ActionType::LIGHT_UP));
	controller.AddKeyBind(KeyAction(KeyAction::Key(GLFW_KEY_KP_9), KeyAction::ActionType::LIGHT_DOWN));
	
	controller.AddKeyBind(KeyAction(KeyAction::Key(GLFW_KEY_KP_1), KeyAction::ActionType::INCREASE_SPEED));
	controller.AddKeyBind(KeyAction(KeyAction::Key(GLFW_KEY_KP_3), KeyAction::ActionType::DECREASE_SPEED));


	return status;
}

int Application::InitializeGLFW(int majorVersion, int minorVersion) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "ERROR::GLFW::" << FAIL_GLFW_CREATE << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	return 1;
}

int Application::InitializeGLEW(bool experimental) {
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "ERROR::GLEW::" << FAIL_GLFW_CREATE << std::endl;
		return -1;
	}

	return 1;
}

void Application::Run() {

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.52f, 0.8f, 0.95f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	Camera camera(6378137.0, 0, 0);
	camera.SetPerspective(45.0, (double)Application::WINDOW_WIDTH / Application::WINDOW_HEIGHT, 0.01, 27000000.0);

	double radius = 6378137.0;
	Tile t1,t2,t3,t4,t5,t6;
	bool normalize = true;
	bool generateNormals = true;
	t1.generate(128, glm::rotate(glm::pi<double>(), glm::dvec3(1.0, 0.0, 0.0)), glm::translate(glm::dvec3(0.0, 0.5, 0.0)), glm::scale(glm::dvec3(radius, radius, radius)), normalize, generateNormals); //Top
	t2.generate(128, glm::rotate(0.0, glm::dvec3(1.0, 0.0, 0.0)), glm::translate(glm::dvec3(0.0, -0.5, 0.0)), glm::scale(glm::dvec3(radius, radius, radius)), normalize, generateNormals); //Bot
	t3.generate(128, glm::rotate(-glm::half_pi<double>(), glm::dvec3(0.0, 0.0, 1.0)), glm::translate(glm::dvec3(-0.5, 0.0, 0.0)), glm::scale(glm::dvec3(radius, radius, radius)), normalize, generateNormals); //Left
	t4.generate(128, glm::rotate(glm::half_pi<double>(), glm::dvec3(0.0, 0.0, 1.0)), glm::translate(glm::dvec3(0.5, 0.0, 0.0)), glm::scale(glm::dvec3(radius, radius, radius)), normalize, generateNormals); //Right
	t5.generate(128, glm::rotate(-glm::half_pi<double>(), glm::dvec3(1.0, 0.0, 0.0)), glm::translate(glm::dvec3(0.0, 0.0, 0.5)), glm::scale(glm::dvec3(radius, radius, radius)), normalize, generateNormals); //Near
	t6.generate(128, glm::rotate(glm::half_pi<double>(), glm::dvec3(1.0, 0.0, 0.0)), glm::translate(glm::dvec3(0.0, 0.0, -0.5)), glm::scale(glm::dvec3(radius, radius, radius)), normalize, generateNormals); //Far

	Shader shader("E:\\Code\\Berg\\Shaders\\terrain.vert", "E:\\Code\\Berg\\Shaders\\terrain.frag");

	glm::vec3 l(0, 1, 0);
	
	double timer = 0.0;
	bool isChanged = false;
	double oldTime = 0.0;

	Controller &controller = Controller::GetInstance();

	while (!glfwWindowShouldClose(window)) {

		//Poll events first
		glfwPollEvents();

		//Set delta time
		double currentTime = glfwGetTime();
		DELTA_TIME = currentTime - oldTime;
		oldTime = currentTime;

		//Set frames per second
		int frames = (int)(1.0 / DELTA_TIME);
		camera.Update((float)DELTA_TIME);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 m, p, v;
		p = camera.GetProjectionMatrix();
		v = camera.GetViewMatrix();


		controller.UpdateContinuousInput();
		
		bool changed = false;
		for (int i = 0; i < controller.GetCount(); ++i) {
			KeyAction::ActionType t = controller.PeekAction();
			if (t == KeyAction::ActionType::LIGHT_BACK) { l.z -= DELTA_TIME; changed = true; }
			if (t == KeyAction::ActionType::LIGHT_FORWARD) { l.z += DELTA_TIME; changed = true; }
			if (t == KeyAction::ActionType::LIGHT_LEFT) { l.x -= DELTA_TIME; changed = true; }
			if (t == KeyAction::ActionType::LIGHT_RIGHT) { l.x += DELTA_TIME; changed = true; }
			if (t == KeyAction::ActionType::LIGHT_UP) { l.y += DELTA_TIME; changed = true; }
			if (t == KeyAction::ActionType::LIGHT_DOWN) { l.y -= DELTA_TIME; changed = true; }

			if (changed) {
				controller.ConsumeAction();
			}
		}

		glm::mat4 relativeToEye(m * v);
		relativeToEye[3][0] = 0;
		relativeToEye[3][1] = 0;
		relativeToEye[3][2] = 0;

		float xp, xl, yp, yl, zp, zl;
		MathHelp::doubleToTwoFloats(camera.GetEyeVector().x, xp, xl);
		MathHelp::doubleToTwoFloats(camera.GetEyeVector().y, yp, yl);
		MathHelp::doubleToTwoFloats(camera.GetEyeVector().z, zp, zl);

		shader.Use();
		glUniform3fv(glGetUniformLocation(shader.program, "light"), 1, value_ptr(l));

		glUniform1i(glGetUniformLocation(shader.program, "logarithmicDepth"), true);
		glUniform1f(glGetUniformLocation(shader.program, "logarithmicConstant"), 0.1);
		glUniform1f(glGetUniformLocation(shader.program, "nearPlaneDistance"), 0.01);

		glUniformMatrix4fv(glGetUniformLocation(shader.program, "relativeToEye"), 1, GL_FALSE, value_ptr(relativeToEye));
		glUniform3fv(glGetUniformLocation(shader.program, "eyeVecHigh"), 1, value_ptr(glm::vec3(xp, yp, zp)));
		glUniform3fv(glGetUniformLocation(shader.program, "eyeVecLow"), 1, value_ptr(glm::vec3(xl, yl, zl)));

		glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, value_ptr(m));
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, value_ptr(v));
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "proj"), 1, GL_FALSE, value_ptr(p));

		t1._tileMesh.Draw(shader);
		t2._tileMesh.Draw(shader);
		t3._tileMesh.Draw(shader);
		t4._tileMesh.Draw(shader);
		t5._tileMesh.Draw(shader);
		t6._tileMesh.Draw(shader);
	
		glfwSwapBuffers(window);
	}

	glfwTerminate();
}