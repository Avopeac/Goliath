#include "Water.h"
#include <Thread/Message.h>

class Water::WaterMessage : Message {
public:
	void virtual process() override {
		ref->_setup();
	}
private:
	Water *ref;
};

Water::Water(const glm::mat4& translation, const glm::mat4& scale, const glm::mat4& rotation)
	: _translation(translation), _scale(scale), _rotation(rotation) {
	_setup();
}

void Water::draw(const Camera& camera, double delta_time) {
	_draw(camera, delta_time, false);
}

void Water::draw_wireframe(const Camera& camera, double delta_time) {
	_draw(camera, delta_time, true);
}

void Water::_draw(const Camera& camera, double delta_time, bool wireframe) {

}

void Water::_setup() {


}