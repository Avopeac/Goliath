#include <cstdlib>
#include <iostream>
#include <memory>
#include "Application.h"
//Default arguments
#ifndef ARGC_DEFAULT
#define ARGC_DEFAULT 1
#endif
//User defined arguments
#ifndef ARGC_USER
#define ARGC_USER 4
#endif
//The argument denoting window width
#ifndef ARGV_WINDOW_WIDTH
#define ARGV_WINDOW_WIDTH 1
#endif
//The argument denoting window height
#ifndef ARGV_WINDOW_HEIGHT
#define ARGV_WINDOW_HEIGHT 2
#endif
//The argument denoting window name
#ifndef ARGV_WINDOW_NAME
#define ARGV_WINDOW_NAME 3
#endif
/// Start the application either by no number of arguments and use standard settings, else with a desired window width and height and name.
int main(int argc, const char *argv[]) {
	std::shared_ptr<Application> application;
	int w = 0, h = 0;
	//Start an application instance with the given program arguments
	if (argc == ARGC_DEFAULT) {
		std::cout << "Setting up program with default arguments.\n" << std::endl;
		application = std::make_shared<Application>();
	}
	else if (argc == ARGC_USER) {
		if ((w = std::atoi(argv[ARGV_WINDOW_WIDTH])) && (h = std::atoi(argv[ARGV_WINDOW_HEIGHT]))) {
			std::cout << "Setting up program with user defined arguments.\n" << std::endl;
			application = std::make_shared<Application>(w, h, argv[ARGV_WINDOW_NAME]);
		}
	}
	//If everythings ok, start an instance
	if (application != nullptr) {
		std::cout << "Initializing application..." << std::endl;
		application->initialize();
		std::cout << "Initialization completed.\n" << std::endl;
		std::cout << "Running application..." << std::endl;
		application->run();
	}
	else std::cout << "Program was sent invalid arguments. " << std::endl;
}