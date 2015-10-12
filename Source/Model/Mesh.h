#pragma once
#include <vector>
#include "..\Application.h"
#include "..\Model\Vertex.h"
#include "..\Model\Texture.h"
#include "..\Model\Shader.h"
///A mesh is the most basic thing that's uploaded to the GPU, it contains vertices, normals, texture coordinates and textures
class Mesh {
public:
	Mesh(std::vector<Vertex>, std::vector<GLuint>, std::vector<Texture>);
	Mesh() = default;
	~Mesh() = default;
	//Mesh data
	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<GLuint> indices;
	//Draw the mesh with a given shader
	void draw(Shader &shader);
	//Draw the mesh with wireframe mode
	void draw_wireframe(Shader &shader);
	//Upload mesh to GPU
	void setup_mesh();
	//Upload new index list 
	void update_indices();
	//Upload new vertex list
	void update_vertices();

private:
	//OpenGL identifiers
	GLuint VAO, VBO, EBO;

};