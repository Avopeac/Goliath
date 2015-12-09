#pragma once
#include <GLM/vec3.hpp>

class DayNight {
public:
	//Singleton
	static DayNight &instance() {
		static DayNight instance;
		return instance;
	}

	glm::dvec3 get_sun();

protected:
	DayNight();

private:
	glm::dvec3 _sun;
};
