#pragma once
#include <vector>
#include "..\Application.h"
#include "..\Drawable.h"
#include "..\Model\Vertex.h"
#include "..\Model\Texture.h"
#include "..\Model\Shader.h"
///A mesh is the most basic thing that's uploaded to the GPU, it contains vertices, normals, texture coordinates and textures
class Mesh : public Drawable{
public:
	Mesh(std::vector<Vertex>, std::vector<GLuint>, std::vector<Texture>);
	Mesh() = default;
	//Mesh data
	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<GLuint> indices;
	//Inherited via Drawable
	void draw(double delta_time) override;
	//Draw the mesh with wireframe mode
	void draw_wireframe(double delta_time) override;
	//Upload mesh to GPU
	void setup_mesh();
	//Upload new index list 
	void update_indices();
	//Upload new vertex list
	void update_vertices();

private:
	//OpenGL identifiers
	GLuint _VAO, _VBO, _EBO;
};