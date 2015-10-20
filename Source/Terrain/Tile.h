#pragma once
#include <memory>
#include "..\Source\Model\Mesh.h"
#include "FractalHeightFunction.h"
///A tile contains mesh information and has methods to generate a plane with some height function
class Tile {
public:
	Tile() = default;
	//Generation methods, with many parameters such as normalization of a mesh, generation of normal vectors and transformations
	void generate(int resolution, const glm::dmat4 &rotation, const glm::highp_dvec3 &translation, const glm::dmat4 &scale, bool normalize, bool gen_normals);
	void generate(int resolution, std::shared_ptr<HeightFunction> function, const glm::dmat4 &rotation, const glm::highp_dvec3 &translation, const glm::dmat4 &scale, bool normalize, bool gen_normals);
	//The tile mesh
	Mesh _mesh;
};