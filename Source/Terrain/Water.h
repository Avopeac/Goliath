#pragma once
#include <View/Drawable.h>
#include <GLM/detail/type_mat.hpp>

class Water : public Drawable {
public:
	Water(double radius, const glm::dmat4 &translation, const glm::dmat4 &rotation, const glm::dmat4 &scale);

	Water() = delete;
	Water(const Water&) = delete;

	virtual void draw(const Camera &camera, double delta_time) override;
	virtual void draw_wireframe(const Camera &camera, double delta_time) override;
	
private:
	Water(float radius, const glm::dmat4 &translation, const glm::dmat4 &rotation, const glm::dmat4 &scale, double lod_level);

	class WaterMessage;

	std::vector<std::unique_ptr<Water>> _children;

	const unsigned int _base_resolution = 512;
	double _water_level;
	double _lod_level;

	const glm::dmat4 _translation;
	const glm::dmat4 _rotation;
	const glm::dmat4 _scale;

	std::vector<glm::dvec3> _vertices;
	std::vector<glm::dvec3> _normals;
	std::vector<GLuint> _indices;

	unsigned int _VAO, _VBO, _EBO;

	bool _setup_done, _gl_setup_done;

	void _init();
	void _setup();
	void _update_lod();
	bool _children_setup_done();
	void _gl_setup();
	void _draw(const Camera &camera, double delta_time, bool wireframe);
};
