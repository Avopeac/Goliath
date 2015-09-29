#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <GLM\glm.hpp>
#include <GLM\gtc\type_ptr.hpp>

#define W_WIDTH 1024
#define W_HEIGHT 768
#define W_NAME "GLFW Application"

class Application
{

public:

	Application(unsigned int width = W_WIDTH, unsigned int height = W_HEIGHT, const char *title = W_NAME);
	~Application() = default;

	int Initialize();
	void Run();
	
	static unsigned int GetWindowHeight() { return WINDOW_HEIGHT; }
	static unsigned int GetWindowWidth() { return WINDOW_WIDTH; }
	static const char* GetWindowTitle() { return WINDOW_TITLE; }
	static double GetDeltaTime() { return DELTA_TIME; }
	
private:

	static unsigned int WINDOW_HEIGHT;
	static unsigned int WINDOW_WIDTH;
	static const char* WINDOW_TITLE;
	static double DELTA_TIME;
	GLFWwindow *window;

	int InitializeGLEW(bool experimental);
	int InitializeGLFW(int majorVersion, int minorVersion);
};