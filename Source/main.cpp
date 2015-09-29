#include <cstdlib>
#include <iostream>
#include "Application.h"
#define ARGC_DEFAULT 1
#define ARGC_USER 4
#define ARGV_WINDOW_WIDTH 1
#define ARGV_WINDOW_HEIGHT 2
#define ARGV_WINDOW_NAME 3
///
/// Start the application either by no number of arguments and use standard settings, else with a desired window width and height and name.
/// Date: 2015-09-25
/// Author: Andreas Larsson
///
int main(int argc, const char *argv[]) {

	Application *application = nullptr;

	int w = 0, h = 0;
	if (argc == ARGC_DEFAULT) {
		application = new Application();
	}
	else if (argc == ARGC_USER) {
		if ((w = std::atoi(argv[ARGV_WINDOW_WIDTH])) && (h = std::atoi(argv[ARGV_WINDOW_HEIGHT]))) {
			application = new Application(w, h, argv[ARGV_WINDOW_NAME]);
		}
	}

	if (application != nullptr) {
		application->Initialize();
		application->Run();
	}
	else {
		std::cout << "MAIN::INVALID::INPUT::ARGUMENTS" << std::endl;
	}

	delete application;
}