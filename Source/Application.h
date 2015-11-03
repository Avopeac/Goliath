#pragma once
#include <string>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <GLM\glm.hpp>
//The default argument window width
#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH 1920
#endif
//The default argument window height
#ifndef WINDOW_HEIGHT
#define WINDOW_HEIGHT 1080
#endif
//The default argument window name
#ifndef WINDOW_NAME
#define WINDOW_NAME "Goliath Planet Renderer"
#endif
class Application
{
public:
	Application(unsigned int width = WINDOW_WIDTH, unsigned int height = WINDOW_HEIGHT, const std::string &title = WINDOW_NAME);
	static unsigned int width, height;
	int initialize();
	void run();
private:
	GLFWwindow *_window_ptr;
	double _delta_time;
	double _elapsed_time;
	double _frames_per_second;
	const std::string _title;
	int initialize_glew(bool experimental);
	int initialize_glfw(int major_version, int minor_version);
};