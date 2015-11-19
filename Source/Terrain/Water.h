#pragma once
#include <View/Drawable.h>

class Water : public Drawable {
public:
	Water(const glm::mat4 &translation, const glm::mat4 &scale, const glm::mat4 &rotation);

	Water() = delete;
	Water(const Water&) = delete;

	virtual void draw(const Camera &camera, double delta_time) override;
	virtual void draw_wireframe(const Camera &camera, double delta_time) override;
	
private:
	class WaterMessage;

	const unsigned int _base_resolution = 64;
	const float _water_level = 4;

	const glm::mat4 _translation;
	const glm::mat4 _scale;
	const glm::mat4 _rotation;

	std::vector<glm::vec3> _vertices;
	std::vector<glm::vec3> _normals;
	std::vector<GLuint> _indices;

	unsigned int _VAO, _VBO, _EBO;

	bool _setup_done, _gl_setup_done;

	void _init();
	void _setup();
	void _gl_setup();
	void _draw(const Camera &camera, double delta_time, bool wireframe);
};
