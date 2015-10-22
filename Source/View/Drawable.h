#pragma once
#include "..\Model\Shader.h"
#include "..\Model\Mesh.h"
#include "..\View\Camera.h"
class Drawable {
public:
	virtual ~Drawable() = default;
	virtual void draw(const Camera &camera, double delta_time) = 0;
	virtual void draw_wireframe(const Camera &camera, double delta_time) = 0;
	//const Shader &get_shader() const { return _shader; }
	//const Mesh &get_mesh() const { return _mesh; }
	virtual void set_shader(const Shader &shader) { _shader = shader; }
protected:
	Drawable() {};
	Shader _shader;
	Mesh _mesh;
};
