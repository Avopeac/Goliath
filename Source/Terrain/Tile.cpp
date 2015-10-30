#include "Tile.h"

void Tile::generate_mesh(unsigned int resolution, float scale) {
	unsigned int i, j;
	float step = 1.0f / resolution;
	for (i = 0; i <= resolution; ++i)
		for (j = 0; j <= resolution; ++j) {
			_mesh.vertices.push_back({ {i, 0, j}, {0, 0, 0}, {i * step, j * step} });
		}
	
	unsigned int size = _mesh.vertices.size();
	for (i = 0; i < size; ++i) {
		
	}
}

void Tile::draw(const Lighting &lighting, const Camera & camera, double delta_time) {
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void Tile::draw_wireframe(const Lighting &lighting, const Camera & camera, double delta_time) {

}
