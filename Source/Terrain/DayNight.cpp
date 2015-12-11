#include "DayNight.h"
#include "Input/Input.h"
#include <GLM/glm.hpp>

DayNight::DayNight() : _sun(0,1,0) {
	TwAddSeparator(Input::_tw_bar, "Day and Night", NULL);
	TwAddVarRW(Input::_tw_bar, "Sun X Direction", TW_TYPE_FLOAT, &_sun.x, " min=-1.0, max=1.0, step=0.001");
	TwAddVarRW(Input::_tw_bar, "Sun Y Direction", TW_TYPE_FLOAT, &_sun.y, " min=-1.0, max=1.0, step=0.001");
	TwAddVarRW(Input::_tw_bar, "Sun Z Direction", TW_TYPE_FLOAT, &_sun.z, " min=-1.0, max=1.0, step=0.001");
}

glm::vec3 DayNight::get_sun() {
	return glm::normalize(_sun);
}
