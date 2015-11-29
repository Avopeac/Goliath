#pragma once
#include <memory>
#include "Model/Shader.h"
#include "Model/Mesh.h"
#include "View/Camera.h"
#include "View/ShaderStore.h"
class Drawable {
public:
	virtual ~Drawable() = default;
	virtual void draw(const Camera &camera, double delta_time) = 0;
	virtual void draw_wireframe(const Camera &camera, double delta_time) = 0;
	virtual void set_shader(std::shared_ptr<Shader> shader) { _shader = shader; }
	Mesh mesh;
protected:
	Drawable() {};
	std::shared_ptr<Shader> _shader;
};
