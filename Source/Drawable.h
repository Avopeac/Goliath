#pragma once
#include "Source\Model\Shader.h"
class Drawable {
public:
	virtual ~Drawable() = default;
	virtual void draw(double delta_time) = 0;
	virtual void set_shader(const Shader &shader) { _shader = shader; }
private:
	Drawable(const Shader &shader) : _shader(shader) {};
	Shader _shader;
};
