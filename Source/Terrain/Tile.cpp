#include <limits>
#include <memory>
#include <iostream>
#include <GLM\gtc\matrix_transform.hpp>
#include "..\Math\MathHelp.h"
#include "Tile.h"

void Tile::generate(int resolution, const glm::dmat4 &rotation, const glm::highp_dvec3 &translation, const glm::dmat4 &scale, bool normalize, bool gen_normals) {
	generate(resolution, nullptr, rotation, translation, scale, normalize, gen_normals);
}

void Tile::generate(int resolution, std::shared_ptr<HeightFunction> function, const glm::dmat4 &rotation, const glm::highp_dvec3 &translation,
	const glm::dmat4 &scale, bool normalize, bool gen_normals) {
	//Loop counters
	int j, i;
	//Step size
	double delta = 1.0 / resolution; 
	//The offset from origin
	double bias = 0.5; 
	//Generate vertex buffer for tile mesh, n + 1 vertices, gives n tiles
	for (j = 0; j <= resolution; ++j) {
		for (i = 0; i <= resolution; ++i) {

			//Next step
			double di = delta * i;
			double dj = delta * j;

			//Vertex position
			glm::highp_dvec4 temp(di - bias, 0, dj - bias, 0.0);
			double height = function->get_value(glm::highp_dvec3(translation.x + temp.x, 0, translation.z + temp.z));
			temp.y += height;

			//Find normal with central difference
			double x_negative = function->get_value(glm::highp_dvec3(translation.x + temp.x - di - bias, 0, translation.z + temp.z));
			double x_positive = function->get_value(glm::highp_dvec3(translation.x + temp.x + di - bias, 0, translation.z + temp.z));
			double z_negative = function->get_value(glm::highp_dvec3(translation.x + temp.x, 0, translation.z + temp.z - dj - bias));
			double z_positive = function->get_value(glm::highp_dvec3(translation.x + temp.x, 0, translation.z + temp.z + dj - bias));

			glm::highp_dvec4 normal(x_negative - x_positive, 2.0, z_negative - z_positive, 0.0);
			//Mesh normalization enables us to "push" out the mesh into a direction
			if (normalize) {
				//Scale is applied afterwards if normalized
				temp = rotation * temp;
				temp = temp + scale * glm::normalize(temp);
				normal = rotation * normal;
				normal = glm::normalize(normal + scale * glm::normalize(normal));
			}
			else {
				//Scale is applied first
				temp = rotation * scale * temp;
				normal = glm::normalize(rotation * scale * normal);
			}
			//Translation is done lastly
			temp.x += translation.x;
			temp.y += translation.y;
			temp.z += translation.z;

			Vertex v;
			//Convert vertex to floating point precision, using 2 floats
			float xp, xl, yp, yl, zp, zl;
			MathHelp::double_to_two_floats(temp.x, xp, xl);
			MathHelp::double_to_two_floats(temp.y, yp, yl);
			MathHelp::double_to_two_floats(temp.z, zp, zl);
			v._position_high = glm::highp_vec3(xp, yp, zp);
			v._position_low = glm::highp_vec3(xl, yl, zl);
			v._texcoord = { di, dj };
			v._normal = { normal.x, normal.y, normal.z };
			//Add to vertex list
			_mesh.vertices.push_back(v);
		}
	}
	//Generate index buffer for tile mesh
	unsigned int step = resolution + 1;
	for (j = 0; j < resolution; ++j) {
		for (i = 0; i < resolution; ++i) {
			unsigned int idx0 = i + (j + 1) * step;
			unsigned int idx1 = i + 1 + j * step;
			_mesh.indices.push_back(i + j * step);
			_mesh.indices.push_back(idx0);
			_mesh.indices.push_back(idx1);
			_mesh.indices.push_back(idx1);
			_mesh.indices.push_back(idx0);
			_mesh.indices.push_back(i + 1 + (j + 1) * step);
		}
	}
}