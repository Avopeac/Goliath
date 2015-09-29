#include <GLM\gtc\matrix_transform.hpp>
#include "Tile.h"
#include "..\Math\MathHelp.h"

void Tile::generate(int resolution, glm::dmat4 &rotation, glm::dmat4 &translation, glm::dmat4 scale, bool normalize, bool genNormals) {

	_currentResolution = resolution;
	_possibleDecimations = (int)glm::log2((double)resolution);

	int j, i;
	double delta = 1.0 / resolution;
	double bias = 0.5;

	//Generate vertex buffer for tile mesh
	for (j = 0; j <= resolution; ++j) {
		for (i = 0; i <= resolution; ++i) {

			double di = delta * i;
			double dj = delta * j;

			glm::dvec4 temp(di - bias, 0, dj - bias, 1);
			temp = translation * rotation * temp;

			if (normalize) {
				temp = temp + scale * glm::normalize(glm::dvec4(temp.x, temp.y, temp.z, 0));
			}

			float xp, xl, yp, yl, zp, zl;
			MathHelp::doubleToTwoFloats(temp.x, xp, xl);
			MathHelp::doubleToTwoFloats(temp.y, yp, yl);
			MathHelp::doubleToTwoFloats(temp.z, zp, zl);

			Vertex v;
			v.positionHigh = glm::vec3(xp, yp, zp);
			v.positionLow = glm::vec3(xl, yl, zl);
			v.texcoord = { di, dj };
			v.normal = { 0, 0, 0 };

			_tileMesh.vertices.push_back(v);
		}
	}

	//Generate index buffer for tile mesh
	unsigned int step = resolution + 1;
	for (j = 0; j < resolution; ++j) {
		for (i = 0; i < resolution; ++i) {

			unsigned int idx0 = i + (j + 1) * step;
			unsigned int idx1 = i + 1 + j * step;

			_tileMesh.indices.push_back(i + j * step);
			_tileMesh.indices.push_back(idx0);
			_tileMesh.indices.push_back(idx1);
			_tileMesh.indices.push_back(idx1);
			_tileMesh.indices.push_back(idx0);
			_tileMesh.indices.push_back(i + 1 + (j + 1) * step);
		}
	}

	if (genNormals) {
		generateNormals();
	}

	_tileMesh.SetupMesh();
}

void Tile::generateNormals() {

	int count = _tileMesh.indices.size();
	for (int j = 0; j < count; j += 3) {

		const int ia(_tileMesh.indices[j + 0]);
		const int ib(_tileMesh.indices[j + 1]);
		const int ic(_tileMesh.indices[j + 2]);

		Vertex &v1(_tileMesh.vertices[ia]);
		Vertex &v2(_tileMesh.vertices[ib]);
		Vertex &v3(_tileMesh.vertices[ic]);

		glm::dvec3 v1c = v1.positionHigh + v1.positionLow;
		glm::dvec3 v2c = v2.positionHigh + v2.positionLow;
		glm::dvec3 v3c = v3.positionHigh + v3.positionLow;

		glm::dvec3 normal(glm::cross(v1c - v2c,
			v3c - v1c));

		_tileMesh.vertices[ia].normal += normal;
		_tileMesh.vertices[ib].normal += normal;
		_tileMesh.vertices[ic].normal += normal;
	}
}