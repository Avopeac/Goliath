#include "DayNight.h"

DayNight::DayNight() : _sun(0,1,0) {
}

glm::dvec3 DayNight::get_sun() {
	return _sun;
}
