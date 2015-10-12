#pragma once
#include <string>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <GLM\glm.hpp>
#include <GLM\gtc\type_ptr.hpp>
//The default argument window width
#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH 1024
#endif
//The default argument window height
#ifndef WINDOW_HEIGHT
#define WINDOW_HEIGHT 768
#endif
//The default argument window name
#ifndef WINDOW_NAME
#define WINDOW_NAME "Goliath Planet Renderer"
#endif
///The application class steers the program in the right direction
class Application
{
public:
	Application(unsigned int width = WINDOW_WIDTH, unsigned int height = WINDOW_HEIGHT, std::string title = WINDOW_NAME);
	//Initialize the application
	int initialize();
	//Run the application
	void run();
	//Static methods
	static unsigned int get_window_height() { return _window_height; }
	static unsigned int get_window_width() { return _window_width; }
	static std::string get_window_title() { return _window_title; }
	static double get_delta_time() { return _delta_time; }
	
private:
	static unsigned int _window_height;
	static unsigned int _window_width;
	static std::string _window_title;
	static double _delta_time;
	//The glfw window pointer
	GLFWwindow *_window_ptr;
	//Initialize glew
	int initialize_glew(bool experimental);
	//Initialize glfw
	int initialize_glfw(int major_version, int minor_version);
};