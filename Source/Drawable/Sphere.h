#pragma once
#include "Primitive.h"
#include "..\View\Drawable.h"
#include "..\Model\Mesh.h"
class Sphere : public Primitive, public Drawable {
public:
	Sphere(const glm::vec3 &origin, double radius);
	void generate_mesh(unsigned int latitudes, unsigned int longitudes);
	double get_radius() const { return _radius; }
	virtual void draw(const Camera &camera, double delta_time) override;
	virtual void draw_wireframe(const Camera &camera, double delta_time) override;
private:
	double _radius;
	Mesh _mesh;

};