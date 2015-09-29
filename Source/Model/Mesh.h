#pragma once
#include <vector>
#include "..\Application.h"
#include "..\Model\Vertex.h"
#include "..\Model\Texture.h"
#include "..\Model\Shader.h"
class Mesh {

public:

	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<GLuint> indices;

	Mesh(std::vector<Vertex>, std::vector<GLuint>, std::vector<Texture>);
	Mesh() = default;
	~Mesh() = default;

	void Draw(Shader &shader);
	void DrawWireframe(Shader &shader);

	void SetupMesh();
	void UpdateIndices();
	void UpdateVertices();

private:
	GLuint VAO, VBO, EBO;

};