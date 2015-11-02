#pragma once
#include <memory>
#include "..\Model\Shader.h"
#include "..\Model\Mesh.h"
#include "..\Model\Material.h"
#include "..\Model\Lighting.h"
#include "..\View\Camera.h"
class Drawable {
public:
	virtual ~Drawable() = default;
	virtual void draw(const Lighting &lighting, const Camera &camera, double delta_time) = 0;
	virtual void draw_wireframe(const Lighting &lighting, const Camera &camera, double delta_time) = 0;
	virtual void set_shader(std::shared_ptr<Shader> shader, bool is_standard) { _shader = shader; uses_standard_shader = is_standard; }
	virtual void set_material(const Material &material) { _material = material; }
protected:
	Drawable() {};
	Material _material;
	std::shared_ptr<Shader> _shader;
	Mesh _mesh;
	bool uses_standard_shader;
};
