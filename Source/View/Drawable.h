#pragma once
#include "..\Model\Shader.h"
#include "..\View\Camera.h"
class Drawable {
public:
	virtual ~Drawable() = default;
	virtual void draw(const Camera &camera, double delta_time) = 0;
	virtual void draw_wireframe(const Camera &camera, double delta_time) = 0;
	virtual Shader &get_shader() { return _shader; }
	virtual void set_shader(const Shader &shader) { _shader = shader; }
protected:
	Drawable() {};
	Shader _shader;
};
