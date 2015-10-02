#pragma once
#include <memory>
#include "..\Math\Cartesian3D.h"
#include "..\Source\Model\Mesh.h"
#include "FractalHeightFunction.h"


class Tile {
public:

	Tile() = default;
	~Tile() = default;

	void generate(int resolution, glm::dmat4 &rotation, glm::dmat4 &translation, glm::dmat4 scale, bool normalize, bool genNormals);
	void generate(int resolution, std::shared_ptr<HeightFunction> function, glm::dmat4 &rotation, glm::dmat4 &translation, glm::dmat4 scale, bool normalize, bool genNormals);
	void generateNormals();

	Mesh _tileMesh;

	double _maxHeight;
	double _minHeight;

private:
	int _currentResolution;
	int _possibleDecimations;
	
};