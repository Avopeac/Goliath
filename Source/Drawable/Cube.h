#pragma once
#include "..\View\Drawable.h"
class Cube : public Drawable {
public:
	Cube() : Drawable() {
		mesh.vertices.push_back({ { -1.0f,  -1.0f, 1.0f } }); //front
		mesh.vertices.push_back({ { 1.0f,  -1.0f, 1.0f } }); //front
		mesh.vertices.push_back({ { 1.0f,  1.0f, 1.0f } }); //front
		mesh.vertices.push_back({ { -1.0f,  1.0f, 1.0f } }); //front
		mesh.vertices.push_back({ { -1.0f,  -1.0f, -1.0f } }); //back
		mesh.vertices.push_back({ { 1.0f,  -1.0f, -1.0f } }); //back
		mesh.vertices.push_back({ { 1.0f,  1.0f, -1.0f } }); //back
		mesh.vertices.push_back({ { -1.0f,  1.0f, -1.0f } }); //back
		mesh.indices.push_back(0); mesh.indices.push_back(1); mesh.indices.push_back(2); //front
		mesh.indices.push_back(2); mesh.indices.push_back(3); mesh.indices.push_back(0); //front
		mesh.indices.push_back(3); mesh.indices.push_back(2); mesh.indices.push_back(6); //top
		mesh.indices.push_back(6); mesh.indices.push_back(7); mesh.indices.push_back(3); //top
		mesh.indices.push_back(7); mesh.indices.push_back(6); mesh.indices.push_back(5); //back
		mesh.indices.push_back(5); mesh.indices.push_back(4); mesh.indices.push_back(7); //back
		mesh.indices.push_back(4); mesh.indices.push_back(5); mesh.indices.push_back(1); //bottom
		mesh.indices.push_back(1); mesh.indices.push_back(0); mesh.indices.push_back(4); //bottom
		mesh.indices.push_back(4); mesh.indices.push_back(0); mesh.indices.push_back(3); //left
		mesh.indices.push_back(3); mesh.indices.push_back(7); mesh.indices.push_back(4); //left
		mesh.indices.push_back(1); mesh.indices.push_back(5); mesh.indices.push_back(6); //right
		mesh.indices.push_back(6); mesh.indices.push_back(2); mesh.indices.push_back(1); //right
		mesh.setup_mesh();
	}
};
