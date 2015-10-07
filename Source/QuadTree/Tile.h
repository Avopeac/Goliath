#pragma once
#include <memory>
#include "..\Math\Cartesian3D.h"
#include "..\Source\Model\Mesh.h"
#include "FractalHeightFunction.h"


class Tile {
public:

	Tile() = default;
	~Tile() = default;

	void generate(int resolution, const glm::dmat4 &rotation, const glm::highp_dvec3 &translation, const glm::dmat4 &scale, bool normalize, bool genNormals);
	void generate(int resolution, std::shared_ptr<HeightFunction> function, const glm::dmat4 &rotation, const glm::highp_dvec3 &translation, const glm::dmat4 &scale, bool normalize, bool genNormals);
	void generateNormals();

	Mesh _tileMesh;

	double _maxHeight;
	double _minHeight;

private:
	int _currentResolution;
	int _possibleDecimations;
	
};