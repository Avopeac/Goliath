#pragma once
#include <memory>
#include "Model/Shader.h"
#include "Model/Mesh.h"
#include "Model/Material.h"
#include "Model/Lighting.h"
#include "View/Camera.h"
#include "View/ShaderStore.h"
class Drawable {
public:
	virtual ~Drawable() = default;
	virtual void draw(const Camera &camera, double delta_time) = 0;
	virtual void draw_wireframe(const Camera &camera, double delta_time) = 0;
	virtual void set_shader(std::shared_ptr<Shader> shader) { _shader = shader; }
	virtual void set_material(const Material &material) { _material = material; }
	Mesh _mesh;
protected:
	Drawable() {};
	Material _material;
	std::shared_ptr<Shader> _shader;

};
