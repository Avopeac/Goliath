#pragma once
#include "Model\Shader.h"
class Drawable {
public:
	virtual ~Drawable() = default;
	virtual void draw(double delta_time) = 0;
	virtual void draw_wireframe(double delta_time) = 0;
	virtual const Shader &get_shader() const { return _shader; }
	virtual void set_shader(const Shader &shader) { _shader = shader; }
protected:
	Drawable() {};
	Shader _shader;
};
