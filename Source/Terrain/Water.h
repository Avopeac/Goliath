#pragma once
#include <View/Drawable.h>
#include <GLM/detail/type_mat.hpp>
#include <mutex>

class Water : public Drawable {
public:
	~Water();
	Water(double radius, const glm::dmat4 &translation, const glm::dmat4 &rotation, const glm::dmat4 &scale);

	Water() = delete;
	Water(const Water&) = delete;

	virtual void draw(const Camera &camera, double delta_time) override;
	virtual void draw_wireframe(const Camera &camera, double delta_time) override;
	
private:
	Water(float radius, const glm::dmat4 &translation, const glm::dmat4 &rotation, const glm::dmat4 &scale, double lod_level);

	class WaterMessage;

	const unsigned int WATER_BASE_RESOLUTION = 16;

	double _water_level;
	double _lod_level;

	int _message_ref;
	std::vector<std::unique_ptr<Water>> _children;

	const glm::dmat4 _translation;
	const glm::dmat4 _rotation;
	const glm::dmat4 _scale;

	glm::dvec3 _center, _extents;

	std::vector<glm::dvec3> _vertices;
	std::vector<glm::dvec3> _normals;
	std::vector<GLuint> _indices;

	unsigned int _VAO, _VBO, _EBO;

	bool _setup_done, _gl_setup_done;

	void _init();
	void _setup();
	void _update_lod(double rho);
	bool _children_setup_done();
	void _gl_setup();
	void _draw(const Camera &camera, double delta_time, bool wireframe);
};
