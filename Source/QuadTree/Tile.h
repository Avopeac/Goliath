#pragma once
#include "..\Math\Cartesian3D.h"
#include "..\Source\Model\Mesh.h"

class Tile {
public:

	Tile() = default;
	~Tile() = default;

	void generate(int resolution, glm::dmat4 &rotation, glm::dmat4 &translation, glm::dmat4 scale, bool normalize, bool genNormals);
	void generateNormals();

	Mesh _tileMesh;

private:
	int _currentResolution;
	int _possibleDecimations;
};