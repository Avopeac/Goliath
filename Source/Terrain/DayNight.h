#pragma once
#include <GLM/vec3.hpp>

class DayNight {
public:
	//Singleton
	static DayNight &instance() {
		static DayNight instance;
		return instance;
	}

	glm::vec3 get_sun();

protected:
	DayNight();

private:
	glm::vec3 _sun;
};
