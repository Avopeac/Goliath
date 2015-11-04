#pragma once
#include "..\View\Drawable.h"
class Cube : public Drawable {
public:
	Cube() : Drawable() {
		_mesh.vertices.push_back({ { -1.0f,  -1.0f, 1.0f } }); //front
		_mesh.vertices.push_back({ { 1.0f,  -1.0f, 1.0f } }); //front
		_mesh.vertices.push_back({ { 1.0f,  1.0f, 1.0f } }); //front
		_mesh.vertices.push_back({ { -1.0f,  1.0f, 1.0f } }); //front
		_mesh.vertices.push_back({ { -1.0f,  -1.0f, -1.0f } }); //back
		_mesh.vertices.push_back({ { 1.0f,  -1.0f, -1.0f } }); //back
		_mesh.vertices.push_back({ { 1.0f,  1.0f, -1.0f } }); //back
		_mesh.vertices.push_back({ { -1.0f,  1.0f, -1.0f } }); //back
		_mesh.indices.push_back(0); _mesh.indices.push_back(1); _mesh.indices.push_back(2); //front
		_mesh.indices.push_back(2); _mesh.indices.push_back(3); _mesh.indices.push_back(0); //front
		_mesh.indices.push_back(3); _mesh.indices.push_back(2); _mesh.indices.push_back(6); //top
		_mesh.indices.push_back(6); _mesh.indices.push_back(7); _mesh.indices.push_back(3); //top
		_mesh.indices.push_back(7); _mesh.indices.push_back(6); _mesh.indices.push_back(5); //back
		_mesh.indices.push_back(5); _mesh.indices.push_back(4); _mesh.indices.push_back(7); //back
		_mesh.indices.push_back(4); _mesh.indices.push_back(5); _mesh.indices.push_back(1); //bottom
		_mesh.indices.push_back(1); _mesh.indices.push_back(0); _mesh.indices.push_back(4); //bottom
		_mesh.indices.push_back(4); _mesh.indices.push_back(0); _mesh.indices.push_back(3); //left
		_mesh.indices.push_back(3); _mesh.indices.push_back(7); _mesh.indices.push_back(4); //left
		_mesh.indices.push_back(1); _mesh.indices.push_back(5); _mesh.indices.push_back(6); //right
		_mesh.indices.push_back(6); _mesh.indices.push_back(2); _mesh.indices.push_back(1); //right
		_mesh.setup_mesh();
	}
};
