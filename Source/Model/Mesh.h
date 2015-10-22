#pragma once
#include <vector>
#include "..\Application.h"
#include "..\Model\Vertex.h"
#include "..\Model\Texture.h"
#include "..\Model\Shader.h"
#include "..\View\Camera.h"
///A mesh is the most basic thing that's uploaded to the GPU, it contains vertices, normals, texture coordinates and textures
class Mesh {
public:
	Mesh(std::vector<Vertex>, std::vector<GLuint>, std::vector<Texture>);
	Mesh() = default;
	//Mesh data
	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<GLuint> indices;
	void draw(const Shader &shader, double delta_time);
	void draw_wireframe(const Shader &shader, double delta_time);
	void setup_mesh();
	void update_indices();
	void update_vertices();

private:
	//OpenGL identifiers
	GLuint _VAO, _VBO, _EBO;
};